import { useState, useEffect } from "react";
import { View, Text, StyleSheet, Button } from "react-native";
import * as ScreenOrientation from "expo-screen-orientation";
import React from "react";
import * as NavigationBar from "expo-navigation-bar";
import { useBleController, type DataFrame } from "../../hooks/useBleController";

export default function HomeScreen() {
  const [dataFrame, setDataFrame] = useState<DataFrame | null>(null);
  const [, setLandscape] = useState(false);
  const [isMirrorView, setMirrorView] = useState(true);
  // const bleController = useBleController();

  useEffect(() => {
    const job = {id: -1}

    function action() {
      fetch("http://localhost:1234").then((result) => 
        result.json()
      ).then((data) => {
        setDataFrame(data);
        // console.log(data);
        // job.id = requestAnimationFrame(action);
        job.id = setTimeout(action, 100);
      }).catch((error) => {
        console.log(error);
      });
    }

    action();
    NavigationBar.setVisibilityAsync("hidden");
    setLandscapeLeftOrientation();
    // bleController.requestPermissions();

    return () => {
      // bleController.disconnect();
      // cancelAnimationFrame(job.id);
      clearTimeout(job.id);
    };
  }, []);

  return (
    <View style={styles.container}>
      <View style={styles.innerContainer}>
        <View style={[styles.valuesColumn, isMirrorView && styles.mirrorValuesColumn]}>
          <Text style={[styles.styledText, styles.speedValueText, isMirrorView && styles.mirrorView]}>
            {dataFrame?.vehicleSpeed}
          </Text>
        </View>
        <View style={[styles.unitsColumn, isMirrorView && styles.mirrorUnitsColumn]}>
          <Text style={[styles.styledText, styles.unitsText, isMirrorView && styles.mirrorView]}>km/h</Text>
          <Button
            title="Switch"
            // onPress={() => bleController.connect(setDataFrame)}
            onPress={() => setMirrorView(!isMirrorView)}
          />
        </View>
        <View style={[styles.valuesColumn, isMirrorView && styles.mirrorValuesColumn]}>
          <Text style={[styles.styledText, styles.rpmValueText, isMirrorView && styles.mirrorView]}>
            {dataFrame?.engineRPM}
          </Text>
        </View>
        <View style={[styles.unitsColumn, isMirrorView && styles.mirrorUnitsColumn]}>
          <Text style={[styles.styledText, styles.unitsText, isMirrorView && styles.mirrorView]}>rpm</Text>
          {/* <Button
            title="Connect"
            onPress={() => bleController.connect(setDataFrame)}
          />
          <Text style={styles.connectionIndicator}>
            {bleController.connectedDevice
              ? "Device connected"
              : "No device connected"}
          </Text> */}
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
    paddingLeft: "8%",
    paddingRight: "22%",
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
    paddingBottom: 75,
    // transform: 
    // [{ scaleY: -1 }],
  },
  mirrorView: {
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
  connectionIndicator: {
    color: "white",
    fontSize: 20,
  },

});
