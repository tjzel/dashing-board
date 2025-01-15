import { useState, useEffect } from "react";
import { View, Text, StyleSheet } from "react-native";
import * as ScreenOrientation from "expo-screen-orientation";
import React from "react";
import * as NavigationBar from "expo-navigation-bar";
import { useBLE } from "../../hooks/useBLE";
import base64 from "react-native-base64";
import { Characteristic } from "react-native-ble-plx";

interface DataFrame {
  engineLoad: number;
  engineRPM: number;
  vehicleSpeed: number;
  uptime: number;
  fuelLevel: number;
  absoluteLoad: number;
  relativeThrottlePosition: number;
  engineFuelRate: number;
}

interface DataCharacteristics {
  update: Characteristic;
  engineLoad: Characteristic;
  engineRPM: Characteristic;
  vehicleSpeed: Characteristic;
  uptime: Characteristic;
  fuelLevel: Characteristic;
  absoluteLoad: Characteristic;
  relativeThrottlePosition: Characteristic;
  engineFuelRate: Characteristic;
}

export default function HomeScreen() {
  const [json, setJson] = useState<DataFrame | null>(null);
  const [chars, setChars] = useState<DataCharacteristics>({});
  const [isLandscape, setLandscape] = useState(false);
  NavigationBar.setVisibilityAsync("hidden");

  const {
    allDevices,
    connectedDevice,
    connectToDevice,
    color,
    requestPermissions,
    scanForPeripherals,
  } = useBLE();

  const scanForDevices = async () => {
    const isPermissionsEnabled = await requestPermissions();
    if (isPermissionsEnabled) {
      scanForPeripherals();
    }
  };

  useEffect(() => {
    allDevices.forEach((device) => {
      device
        .connect()
        .then((connection) => {
          return connection.discoverAllServicesAndCharacteristics();
        })
        .then((discovered) => {
          // console.log(discovered.serviceData);
          // console.log(discovered.serviceUUIDs);
          // return discovered.characteristicsForService(discovered.serviceUUIDs[2]);
          return discovered.services();
        })
        .then((services) => {
          services.forEach((service) => {
            console.log(service.uuid);
            service.characteristics().then((characteristics) => {
              characteristics.forEach((characteristic) => {
                console.log(characteristic.uuid);
                characteristic.readDescriptor("2901").then((read) => {
                  // console.log(read);
                  const name = base64.decode(read.value);
                  // console.log(name);
                  // console.log(base64.decode(read.value));
                  const newChars = {};
                  if (name === "update") {
                    // setChars((prevChars) => ({
                    //   ...prevChars,
                    //   update: characteristic,
                    // }));
                    // newChars.update = characteristic;
                  } else if (name === "engineLoad") {
                    // setChars((prevChars) => ({
                    //   ...prevChars,
                    //   engineLoad: characteristic,
                    // }));
                    // newChars.engineLoad = characteristic;
                  } else if (name === "engineRPM") {
                    console.log('setting engineRPM');
                    // setChars((prevChars) => ({
                    //   ...prevChars,
                    //   engineRPM: characteristic,
                    // }));
                    newChars.engineRPM = characteristic;
                  } else if (name === "vehicleSpeed") {
                    // setChars((prevChars) => ({
                    //   ...prevChars,
                    //   vehicleSpeed: characteristic,
                    // }));
                    // newChars.vehicleSpeed = characteristic;
                  } else if (name === "uptime") {
                    // setChars((prevChars) => ({
                    //   ...prevChars,
                    //   uptime: characteristic,
                    // }));
                    // newChars.uptime = characteristic;
                  } else if (name === "fuelLevel") {
                    // setChars((prevChars) => ({
                    //   ...prevChars,
                    //   fuelLevel: characteristic,
                    // }));
                    // newChars.fuelLevel = characteristic;
                  } else if (name === "absoluteLoad") {
                    // setChars((prevChars) => ({
                    //   ...prevChars,
                    //   absoluteLoad: characteristic,
                    // }));
                    // newChars.absoluteLoad = characteristic;
                  } else if (name === "relativeThrottlePosition") {
                    // setChars((prevChars) => ({
                    //   ...prevChars,
                    //   relativeThrottlePosition: characteristic,
                    // }));
                    // newChars.relativeThrottlePosition = characteristic;
                  } else if (name === "engineFuelRate") {
                    // setChars((prevChars) => ({
                    //   ...prevChars,
                    //   engineFuelRate: characteristic,
                    // }));
                    // newChars.engineFuelRate = characteristic;
                  }
                  setChars(newChars);
                });
                // console.log(characteristic.value);
                // console.log(characteristic.descriptors);
              });
            });
          });
        });
    });
  }, [allDevices]);

  useEffect(() => {
    setLandscapeLeftOrientation();
    scanForDevices();
    let job = { id: -1 };
    const action = () => {
      // console.log("action")
      // fetch("http://localhost:1234/")
      //   .then((response) => response.json())
      //   .then((data) => {
      //     setJson(data);
      //     job.id = requestAnimationFrame(action);
      //   })
      //   .catch((error) => console.log(error));
      const blePromises = [Promise.resolve()];

      if (chars?.engineRPM) {
        console.log("asking for RPM");
        blePromises.engineRPM = chars?.engineRPM?.read().then((data) => {
          return { engineRPM: base64.decode(data.value) };
        });
      }
      if (chars?.vehicleSpeed) {
        blePromises.vehicleSpeed = chars?.vehicleSpeed?.read().then((data) => {
          return { vehicleSpeed: base64.decode(data.value) };
        });
      }
      if (chars?.engineLoad) {
        // chars?.engineLoad?.read().then((data) => {
        //   // console.log(base64.decode(data.value));
        //   bleJson.engineLoad = base64.decode(data.value);
        //   setJson(bleJson);
        // });
        blePromises.engineLoad = chars?.engineLoad?.read().then((data) => {
          return { engineLoad: base64.decode(data.value) };
        });
      }
      if (chars?.engineFuelRate) {
        // chars?.engineFuelRate?.read().then((data) => {
        //   // console.log(base64.decode(data.value));
        //   bleJson.engineFuelRate = base64.decode(data.value);
        //   setJson(bleJson);
        // });
        blePromises.engineFuelRate = chars?.engineFuelRate?.read().then((data) => {
          return { engineFuelRate: base64.decode(data.value) };
        });
      }
      if (chars?.uptime) {
        // chars?.uptime?.read().then((data) => {
        //   // console.log(base64.decode(data.value));
        //   bleJson.uptime = base64.decode(data.value);
        //   setJson(bleJson);
        // });
        blePromises.uptime = chars?.uptime?.read().then((data) => {
          return { uptime: base64.decode(data.value) };
        });
      }
      if (chars?.fuelLevel) {
        // chars?.fuelLevel?.read().then((data) => {
        //   // console.log(base64.decode(data.value));
        //   bleJson.fuelLevel = base64.decode(data.value);
        //   setJson(bleJson);
        // });
        blePromises.fuelLevel = chars?.fuelLevel?.read().then((data) => {
          return { fuelLevel: base64.decode(data.value) };
        });
      }
      if (chars?.absoluteLoad) {
        // chars?.absoluteLoad?.read().then((data) => {
        //   // console.log(base64.decode(data.value));
        //   bleJson.absoluteLoad = base64.decode(data.value);
        //   setJson(bleJson);
        // });
        blePromises.absoluteLoad = chars?.absoluteLoad?.read().then((data) => {
          return { absoluteLoad: base64.decode(data.value) };
        });
      }
      if (chars?.relativeThrottlePosition) {
        // chars?.relativeThrottlePosition?.read().then((data) => {
        //   // console.log(base64.decode(data.value));
        //   bleJson.relativeThrottlePosition = base64.decode(data.value);
        //   setJson(bleJson);
        // });
        blePromises.relativeThrottlePosition = chars?.relativeThrottlePosition?.read().then((data) => {
          return { relativeThrottlePosition: base64.decode(data.value) };
        });
      }
      // setJson(bleJson);
      let bleJson = {};
      // console.log("looks promising");
      Promise.all(blePromises).then((data) => {
          bleJson = {...bleJson, ...data};
          // console.log('lets set');
          setJson(bleJson);''
          job.id = requestAnimationFrame(action);
        }).catch((error) => console.log(error));

    };
    action();
     return () => cancelAnimationFrame(job.id);
  }, [chars]);

  const setLandscapeLeftOrientation = async () => {
    await ScreenOrientation.lockAsync(
      ScreenOrientation.OrientationLock.LANDSCAPE_LEFT
    );
    setLandscape(true);
  };

  return (
    <View style={styles.container}>
      <View style={styles.innerContainer}>
        <View style={styles.valuesColumn}>
          <Text style={[styles.styledText, styles.speedValueText]}>
            {json?.vehicleSpeed}
          </Text>
        </View>
        <View style={styles.unitsColumn}>
          <Text style={[styles.styledText, styles.unitsText]}>km/h</Text>
        </View>
        <View style={styles.valuesColumn}>
          <Text style={[styles.styledText, styles.rpmValueText]}>
            {json?.engineRPM}
          </Text>
        </View>
        <View style={styles.unitsColumn}>
          <Text style={[styles.styledText, styles.unitsText]}>rpm</Text>
        </View>
      </View>
    </View>
  );
}

const styles = StyleSheet.create({
  container: {
    flex: 1,
    backgroundColor: "black",
  },
  innerContainer: {
    flex: 1,
    flexDirection: "row",
    paddingLeft: "8%",
    paddingRight: "22%",
  },
  valuesColumn: {
    flex: 2,
    justifyContent: "flex-start",
    alignItems: "flex-end",
  },
  unitsColumn: {
    justifyContent: "flex-start",
  },
  styledText: {
    fontSize: 75,
    color: "white",
    fontFamily: "Droid 1997",
    textAlignVertical: "bottom",
    paddingBottom: 75,
    transform: [{ scaleY: -1 }],
  },
  speedValueText: {
    fontSize: 155,
  },
  rpmValueText: {
    fontSize: 110,
  },
  unitsText: {
    paddingBottom: 87,
    paddingLeft: 10,
    color: "#c5c8c8",
  },
});
