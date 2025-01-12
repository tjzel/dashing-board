import { useState, useEffect } from "react";
import { View, Text, StyleSheet } from "react-native";
import * as ScreenOrientation from "expo-screen-orientation";
import React from "react";
import * as NavigationBar from "expo-navigation-bar";

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

export default function HomeScreen() {
  const [json, setJson] = useState<DataFrame | null>(null);
  const [isLandscape, setLandscape] = useState(false);
  NavigationBar.setVisibilityAsync("hidden");

  useEffect(() => {
    setLandscapeLeftOrientation();
    let job = -1;
    const action = () => {
      fetch("http://localhost:1234/")
        .then((response) => response.json())
        .then((data) => {
          setJson(data);
          job = requestAnimationFrame(action);
        })
        .catch((error) => console.log(error));
    };
    action();
    return () => cancelAnimationFrame(job);
  }, []);

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
