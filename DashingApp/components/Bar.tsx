import React from 'react';
import { View, StyleSheet } from 'react-native';
import Svg, { Path } from 'react-native-svg';

export function Bar({length}: {length: number}) {

  return (
    <View style={styles.container}>
      <Svg width="400" height="50" viewBox="0 0 400 50">
        <Path
          // d={`M 20,0 A 90,90 0 1,0 180,0`}
          d={`M 0,25 L ${length*400},25 Z`}
          fill="none"
          stroke="white"
          strokeWidth="50"
        />
          </Svg>
        {/* <Svg height="50" width="200" viewBox="0 0 100 50"> */}
        {/* <Path
          d="M 100,0 
             A 0,50 0 0,1 100,50"
          fill="none"
          stroke="white"
          strokeWidth="2"
        />

      {/* </Svg> */}
    </View>
  );
};

const styles = StyleSheet.create({
  container: {
    // alignItems: 'center',
    // justifyContent: 'center',
    // marginTop: 50,
    // position: 'absolute',
    position: 'absolute',
    borderColor: 'white',
    borderWidth: 1,
    transform: [{ translateY: 370 }, { translateX: 150 }],
  },
});
