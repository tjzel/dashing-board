import { useRef, useState } from "react";
import { PermissionsAndroid, Platform } from "react-native";
import { platformApiLevel } from "expo-device";
import base64 from "react-native-base64";
import { BleManager, Device } from "react-native-ble-plx";

export interface DataFrame {
  engineLoad: number;
  engineRPM: number;
  vehicleSpeed: number;
  uptime: number;
  fuelLevel: number;
  absoluteLoad: number;
  relativeThrottlePosition: number;
  engineFuelRate: number;
}

const bleManager = new BleManager();

export function useBleController(disabled?: boolean) {
  const discoveredDevices = useRef<Device[]>([]);
  const [connectedDevice, setConnectedDevice] = useState<Device | null>(null);

  if (disabled) {
    return null;
  }

  if (!connectedDevice) {
    scanForDevices();
  }

  return {
    connect,
    disconnect,
    connectedDevice,
    requestPermissions,
  };

  async function connect(onUpdate: (data: DataFrame) => void) {
    if (connectedDevice && (await connectedDevice.isConnected())) {
      console.log("Device already connected");
      return true;
    }

    await bleManager.stopDeviceScan();
    while (discoveredDevices.current.length > 0) {
      const device = discoveredDevices.current.pop()!;

      let connectedDevice;
      try {
        connectedDevice = await device.connect({ requestMTU: 512 });
        connectedDevice.onDisconnected(() => setConnectedDevice(null));
        await connectedDevice.discoverAllServicesAndCharacteristics();
      } catch (e) {
        console.log("FAILED TO CONNECT", e);
        continue;
      }

      const controllerCharacteristic = await getControllerCharacteristic(
        connectedDevice
      );
      if (!controllerCharacteristic) {
        continue;
      }

      controllerCharacteristic.monitor((data, char) => {
        console.log(base64.decode(char!.value!));
        onUpdate(JSON.parse(base64.decode(char!.value!)));
      });

      setConnectedDevice(connectedDevice);
      return true;
    }

    console.log("No device to connect to. Try again later.");
    scanForDevices();
    return false;
  }

  async function disconnect() {
    if (!connectedDevice) {
      return;
    }

    await connectedDevice.cancelConnection();
    setConnectedDevice(null);
    scanForDevices();
  }

  async function scanForDevices() {
    bleManager.startDeviceScan(null, null, (error, device) => {
      const targetDeviceName = "DashingBoard";
      if (error) {
        console.log(error);
      } else if (
        device &&
        (device.localName === targetDeviceName ||
          device.name === targetDeviceName)
      ) {
        if (isDuplicateDevice(discoveredDevices.current, device)) {
          return;
        }
        discoveredDevices.current.push(device);
      }
    });
  }
}

async function getControllerCharacteristic(device: Device) {
  try {
    const services = await device.services();
    for (const service of services) {
      const characteristics = await service.characteristics();
      for (const characteristic of characteristics) {
        const labelUUID = "00002901-0000-1000-8000-00805f9b34fb";

        const descriptors = await characteristic.descriptors();
        const labelDescriptor = descriptors.find(
          (descriptor) => descriptor.uuid === labelUUID
        );

        if (!labelDescriptor) {
          continue;
        }

        const rawName = (await labelDescriptor.read()).value;

        if (!rawName) {
          continue;
        }

        const name = base64.decode(rawName);
        if (name === "json") {
          return characteristic;
        }
      }
    }
  } catch (e) {
    console.log("FAILED TO FIND CONTROLLER SERVICE", e);
  }

  return null;
}

async function requestPermissions() {
  if (Platform.OS === "android") {
    if ((platformApiLevel ?? -1) < 31) {
      const granted = await PermissionsAndroid.request(
        PermissionsAndroid.PERMISSIONS.ACCESS_FINE_LOCATION,
        {
          title: "Location Permission",
          message: "Bluetooth Low Energy requires Location",
          buttonPositive: "OK",
        }
      );
      return granted === PermissionsAndroid.RESULTS.GRANTED;
    } else {
      const isAndroid31PermissionsGranted = await requestAndroid31Permissions();
      return isAndroid31PermissionsGranted;
    }
  } else {
    return true;
  }
}

async function requestAndroid31Permissions() {
  const bluetoothScanPermission = await PermissionsAndroid.request(
    PermissionsAndroid.PERMISSIONS.BLUETOOTH_SCAN,
    {
      title: "Location Permission",
      message: "Bluetooth Low Energy requires Location",
      buttonPositive: "OK",
    }
  );
  const bluetoothConnectPermission = await PermissionsAndroid.request(
    PermissionsAndroid.PERMISSIONS.BLUETOOTH_CONNECT,
    {
      title: "Location Permission",
      message: "Bluetooth Low Energy requires Location",
      buttonPositive: "OK",
    }
  );
  const fineLocationPermission = await PermissionsAndroid.request(
    PermissionsAndroid.PERMISSIONS.ACCESS_FINE_LOCATION,
    {
      title: "Location Permission",
      message: "Bluetooth Low Energy requires Location",
      buttonPositive: "OK",
    }
  );

  return (
    bluetoothScanPermission === "granted" &&
    bluetoothConnectPermission === "granted" &&
    fineLocationPermission === "granted"
  );
}

function isDuplicateDevice(devices: Device[], nextDevice: Device) {
  return devices.findIndex((device) => nextDevice.id === device.id) > -1;
}
