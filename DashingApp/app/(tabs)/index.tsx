import { useState, useEffect } from "react";
import { View, Text, StyleSheet, Button, TouchableOpacity } from "react-native";
import * as ScreenOrientation from "expo-screen-orientation";
import React from "react";
import * as NavigationBar from "expo-navigation-bar";
import { useBleController, type DataFrame } from "../../hooks/useBleController";
import { useKeepAwake } from "expo-keep-awake";
import { Arc } from "@/components/Arc";

export default function HomeScreen() {
  useKeepAwake();
  const [dataFrame, setDataFrame] = useState<DataFrame | null>(null);
  const [, setLandscape] = useState(false);
  const [isMirrorView, setMirrorView] = useState(true);
  const [bleDisabled, setBleDisabled] = useState(false);
  const [httpDisabled, setHttpDisabled] = useState(true);
  const bleController = useBleController(bleDisabled);

  useEffect(() => {
    NavigationBar.setVisibilityAsync("hidden");
    // NavigationBar.setBehaviorAsync('inset-touch');

    setLandscapeLeftOrientation();
  }, []);

  useEffect(() => {
    if (!bleController || bleDisabled) {
      return;
    }
    bleController.requestPermissions();
    const bleControllerRef = bleController;
    return () => {
      bleControllerRef.disconnect();
    };
  }, [bleController, bleDisabled]);

  useEffect(() => {
    if (!bleController || bleDisabled) {
      return;
    }
    if (bleController.connectedDevice) {
      return;
    }
    bleController.connect(setDataFrame);
    setTimeout(async () => await bleController.connect(setDataFrame), 1000);
  }, [bleController, bleController?.connectedDevice, bleDisabled]);

  useEffect(() => {
    if (httpDisabled) {
      return;
    }

    const action = () => {
      fetch("http://localhost:1234")
        .then((result) => result.json())
        .then((data) => {
          setDataFrame(data);
          // console.log(data);
          // job.id = requestAnimationFrame(action);
          // job.id = setTimeout(action, 500);
        })
        .catch((error) => {
          console.log(error);
        });
    };

    const interval = setInterval(action, 50);
    NavigationBar.setVisibilityAsync("hidden");
    setLandscapeLeftOrientation();

    return () => {
      clearInterval(interval);
    };
  }, [httpDisabled]);

  const finalGearRatio = 4.105;
  const firstGearRatio = 3.454;
  const secondGearRatio = 1.904;
  const thirdGearRatio = 1.28;
  const fourthGearRatio = 0.966;
  const fifthGearRatio = 0.757;

  const tireCircumference = 14 * 2.4 * 0.0254 * Math.PI;

  const kphToRpm = (1000 / 60 / tireCircumference) * 2;

  let gear = "";

  if (
    dataFrame?.vehicleSpeed !== undefined &&
    dataFrame?.engineRPM !== undefined
  ) {
    const vehicleSpeed = dataFrame.vehicleSpeed;
    const engineRPM = dataFrame.engineRPM;

    const gearlessRPM = vehicleSpeed * kphToRpm;

    gear = (engineRPM / gearlessRPM / finalGearRatio).toFixed(2);

    // if (engineRPM / gearlessRPM >= finalGearRatio * (secondGearRatio + 0.2)) {
    //   gear = "1";
    // } else if (
    //   engineRPM / gearlessRPM >=
    //   finalGearRatio * (thirdGearRatio + 0.2)
    // ) {
    //   gear = "2";
    // } else if (
    //   engineRPM / gearlessRPM >=
    //   finalGearRatio * (fourthGearRatio + 0.1)
    // ) {
    //   gear = "3";
    // } else if (
    //   engineRPM / gearlessRPM >=
    //   finalGearRatio * (fifthGearRatio + 0.1)
    // ) {j
    //   gear = "4";
    // } else {
    //   gear = "5";
    // }
  }

  console.log("gear: ", gear);

  return (
    <View style={styles.container}>
      <View style={styles.innerContainer}>
        <View
          style={[
            styles.valuesColumn,
            isMirrorView && styles.mirrorValuesColumn,
          ]}
        >
          {/* <Text
            style={[
              styles.styledText,
              styles.speedValueText,
              isMirrorView && styles.mirrorView,
              { zIndex: 2 },
            ]}
          >
            {dataFrame?.vehicleSpeed}
          </Text> */}
          <View style={{ flexDirection: "row" }}>
            <View style={{ flexDirection: "column" }}>
              <Text
                style={[
                  styles.styledText,
                  styles.speedValueText,
                  isMirrorView && styles.mirrorView,
                  { zIndex: 2, opacity: 0.1 },
                ]}
              >
                {"0"}
              </Text>
              <Text
                style={[
                  styles.styledText,
                  styles.speedValueText,
                  isMirrorView && styles.mirrorView,
                  { zIndex: 2 },
                  { position: "absolute" },
                ]}
              >
                {dataFrame?.vehicleSpeed
                  ? dataFrame.vehicleSpeed >= 100
                    ? Math.floor((dataFrame.vehicleSpeed / 100) % 10)
                    : ""
                  : ""}
              </Text>
            </View>
            <View style={{ flexDirection: "column" }}>
              <Text
                style={[
                  styles.styledText,
                  styles.speedValueText,
                  isMirrorView && styles.mirrorView,
                  { zIndex: 2, opacity: 0.1 },
                ]}
              >
                {"0"}
              </Text>
              <Text
                style={[
                  styles.styledText,
                  styles.speedValueText,
                  isMirrorView && styles.mirrorView,
                  { zIndex: 2 },
                  { position: "absolute" },
                ]}
              >
                {dataFrame?.vehicleSpeed
                  ? dataFrame.vehicleSpeed >= 10
                    ? Math.floor(dataFrame.vehicleSpeed / 10) % 10
                    : ""
                  : ""}
              </Text>
            </View>
            <View style={{ flexDirection: "column" }}>
              <Text
                style={[
                  styles.styledText,
                  styles.speedValueText,
                  isMirrorView && styles.mirrorView,
                  { zIndex: 2, opacity: 0.1 },
                ]}
              >
                {"0"}
              </Text>
              <Text
                style={[
                  styles.styledText,
                  styles.speedValueText,
                  isMirrorView && styles.mirrorView,
                  { zIndex: 2 },
                  { position: "absolute" },
                ]}
              >
                {typeof dataFrame?.vehicleSpeed === "number"
                  ? dataFrame.vehicleSpeed >= 0
                    ? dataFrame.vehicleSpeed % 10
                    : ""
                  : ""}
              </Text>
            </View>
          </View>
          {/* <Arc length={1}/> */}
          {/* <Bar length={1*((dataFrame?.engineRPM ?? 0)/7000)}/> */}
          <Arc
            length={1 * ((dataFrame?.engineRPM ?? 0) / 6000)}
            greenThreshold={1600 / 7000}
            blueThreshold={2600 / 7000}
            redThreshold={3600 / 7000}
          />
        </View>
        <View
          style={[styles.unitsColumn, isMirrorView && styles.mirrorUnitsColumn]}
        >
          <Text
            style={[
              styles.styledText,
              styles.unitsText,
              isMirrorView && styles.mirrorView,
            ]}
          >
            km/h
          </Text>
          {bleController?.connectedDevice ? null : (
            // {true ? null : (
            <Button
              title="Switch"
              onPress={() => setMirrorView(!isMirrorView)}
            />
          )}
        </View>
        <View style={{ flexDirection: "row" }}>
          <View style={{ flexDirection: "column" }}>
            <Text
              style={[
                styles.styledText,
                styles.rpmValueText,
                isMirrorView && styles.mirrorView,
                { zIndex: 2, opacity: 0.1 },
              ]}
            >
              {"0"}
            </Text>
            <Text
              style={[
                styles.styledText,
                styles.rpmValueText,
                isMirrorView && styles.mirrorView,
                { zIndex: 2 },
                { position: "absolute" },
              ]}
            >
              {dataFrame?.engineRPM
                ? dataFrame.engineRPM >= 1000
                  ? Math.floor(dataFrame.engineRPM / 1000) % 10
                  : ""
                : ""}
            </Text>
          </View>
          <View style={{ flexDirection: "column" }}>
            <Text
              style={[
                styles.styledText,
                styles.rpmValueText,
                isMirrorView && styles.mirrorView,
                { zIndex: 2, opacity: 0.1 },
              ]}
            >
              {"0"}
            </Text>
            <Text
              style={[
                styles.styledText,
                styles.rpmValueText,
                isMirrorView && styles.mirrorView,
                { zIndex: 2 },
                { position: "absolute" },
              ]}
            >
              {dataFrame?.engineRPM
                ? dataFrame.engineRPM >= 100
                  ? Math.floor((dataFrame.engineRPM / 100) % 10)
                  : ""
                : ""}
            </Text>
            <Text
              style={[
                styles.styledText,
                styles.rpmValueText,
                isMirrorView && styles.mirrorView,
                {
                  zIndex: 2,
                  fontSize: 140,
                  position: "absolute",
                  transform: [
                    { translateY: 200 },
                    ...styles.mirrorView.transform,
                  ],
                  width: 200,
                },
              ]}
            >
              {gear}
            </Text>
          </View>
          <View style={{ flexDirection: "column" }}>
            <Text
              style={[
                styles.styledText,
                styles.rpmValueText,
                isMirrorView && styles.mirrorView,
                { zIndex: 2, opacity: 0.1 },
              ]}
            >
              {"0"}
            </Text>
            <Text
              style={[
                styles.styledText,
                styles.rpmValueText,
                isMirrorView && styles.mirrorView,
                { zIndex: 2 },
                { position: "absolute" },
              ]}
            >
              {dataFrame?.engineRPM
                ? dataFrame.engineRPM >= 10
                  ? Math.floor(dataFrame.engineRPM / 10) % 10
                  : ""
                : ""}
            </Text>
          </View>
          <View style={{ flexDirection: "column" }}>
            <Text
              style={[
                styles.styledText,
                styles.rpmValueText,
                isMirrorView && styles.mirrorView,
                { zIndex: 2, opacity: 0.1 },
              ]}
            >
              {"0"}
            </Text>
            <Text
              style={[
                styles.styledText,
                styles.rpmValueText,
                isMirrorView && styles.mirrorView,
                { zIndex: 2 },
                { position: "absolute" },
              ]}
            >
              {dataFrame?.engineRPM
                ? dataFrame.engineRPM >= 0
                  ? dataFrame.engineRPM % 10
                  : ""
                : ""}
            </Text>
          </View>

          {/* <View
          style={[
            styles.valuesColumn,
            isMirrorView && styles.mirrorValuesColumn,
          ]}
        >
          <Text
            style={[
              styles.styledText,
              styles.rpmValueText,
              isMirrorView && styles.mirrorView,
            ]}
          >
            {dataFrame?.engineRPM}
          </Text>
        </View> */}
          <View style={{ flexDirection: "column" }}></View>
        </View>
        <View
          style={[styles.unitsColumn, isMirrorView && styles.mirrorUnitsColumn]}
        >
          <Text
            style={[
              styles.styledText,
              styles.unitsText,
              isMirrorView && styles.mirrorView,
            ]}
          >
            rpm
          </Text>
          {bleController?.connectedDevice ? null : (
            // {/* {true ? null : ( */}
            <Button
              title="Connect"
              onPress={() => {
                bleController?.connect(setDataFrame);
              }}
            />
          )}
          {bleController?.connectedDevice ? null : (
            // {true ? null : (
            <Text style={styles.connectionIndicator}>
              {bleController?.connectedDevice
                ? "Device connected"
                : "No device connected"}
            </Text>
          )}
          {bleController?.connectedDevice ? null : httpDisabled ? (
            <Button
              title="Use HTTP"
              onPress={() => {
                setBleDisabled(true);
                setHttpDisabled(false);
              }}
            />
          ) : (
            <Button
              title="Use BLE"
              onPress={() => {
                setBleDisabled(false);
                setHttpDisabled(true);
              }}
            />
          )}
        </View>
      </View>
    </View>
  );

  async function setLandscapeLeftOrientation() {
    await ScreenOrientation.lockAsync(
      ScreenOrientation.OrientationLock.LANDSCAPE_LEFT
    );
    setLandscape(true);
  }
}

const styles = StyleSheet.create({
  container: {
    flex: 1,
    backgroundColor: "black",
  },
  innerContainer: {
    flex: 1,
    flexDirection: "row",
    paddingLeft: "10%",
    paddingRight: "15%",
  },
  switchButtonColumn: {
    flex: 1,
    justifyContent: "flex-end",
  },
  valuesColumn: {
    flex: 2,
    justifyContent: "flex-end",
    alignItems: "flex-end",
  },
  unitsColumn: {
    justifyContent: "flex-end",
  },
  mirrorValuesColumn: {
    justifyContent: "flex-start",
    alignItems: "flex-end",
  },
  mirrorUnitsColumn: {
    justifyContent: "flex-start",
  },
  styledText: {
    fontSize: 75,
    color: "white",
    fontFamily: "Droid 1997",
    textAlignVertical: "bottom",
    paddingBottom: 0,
  },
  mirrorView: {
    transform: [{ scaleY: -1 }],
    transformOrigin: "center",
  },
  speedValueText: {
    fontSize: 255,
  },
  rpmValueText: {
    fontSize: 180,
    paddingBottom: 14,
  },
  unitsText: {
    paddingBottom: 30,
    paddingLeft: 10,
    color: "#c5c8c8",
  },
  connectionIndicator: {
    color: "white",
    fontSize: 20,
  },
});
