import React, { useEffect } from "react";
import { View, StyleSheet, type ViewStyle } from "react-native";
import Svg, { Path } from "react-native-svg";
import Animated, {
  useAnimatedProps,
  useSharedValue,
  withSpring,
} from "react-native-reanimated";

const AnimatedPath = Animated.createAnimatedComponent(Path);

Animated.addWhitelistedUIProps({
  d: true,
});

export function Arc({
  length,
  greenThreshold,
  blueThreshold,
  redThreshold,
  style,
}: {
  length: number;
  greenThreshold: number;
  blueThreshold: number;
  redThreshold: number;
  style?: ViewStyle;
}) {
  length = length > 1 ? 1 : length < 0 ? 0 : length;

  const lengthSv = useSharedValue(length);

  useEffect(() => {
    lengthSv.value = withSpring(length, { duration: 50 });
  }, [length]);

  const maxArc = 240 / 180;
  const startArc = -210 / 180;
  const gap = 0.02;

  const greyAnimatedProps = useAnimatedProps(() => {
    const length = lengthSv.value;

    const greyLength = length < greenThreshold ? length : greenThreshold;

    const longArc = greyLength * maxArc * 180 >= 180 ? 1 : 0;

    const greyArc = startArc + greyLength * maxArc;

    const greyArcBeginX = 90 * Math.cos(startArc * Math.PI);
    const greyArcBeginY = 90 * Math.sin(startArc * Math.PI);

    const greyArcEndX = 90 * Math.cos(greyArc * Math.PI);
    const greyArcEndY = 90 * Math.sin(greyArc * Math.PI);

    return {
      d: `M ${greyArcBeginX},${greyArcBeginY} A 90,90 0 ${longArc} 1 ${greyArcEndX},${greyArcEndY}`,
    };
  });

  const greenAnimatedProps = useAnimatedProps(() => {
    const length = lengthSv.value;

    const greenLength =
      length < greenThreshold + gap
        ? 0
        : length > blueThreshold
        ? blueThreshold - (greenThreshold + gap)
        : length - (greenThreshold + gap);

    const longArc = greenLength * maxArc * 180 >= 180 ? 1 : 0;

    const greenStartArc = startArc + (greenThreshold + gap) * maxArc;

    const greenArc = greenStartArc + greenLength * maxArc;

    const greenArcBeginX = 90 * Math.cos(greenStartArc * Math.PI);
    const greenArcBeginY = 90 * Math.sin(greenStartArc * Math.PI);

    const greenArcEndX = 90 * Math.cos(greenArc * Math.PI);
    const greenArcEndY = 90 * Math.sin(greenArc * Math.PI);

    return {
      d: `M ${greenArcBeginX},${greenArcBeginY} A 90,90 0 ${longArc} 1 ${greenArcEndX},${greenArcEndY}`,
    };
  });

  const blueAnimatedProps = useAnimatedProps(() => {
    const length = lengthSv.value;

    // const blueLength = length < blueThreshold + gap ? 0 : length > redThreshold ? redThreshold - blueThreshold : length - blueThreshold;
    const blueLength =
      length < blueThreshold + gap
        ? 0
        : length > redThreshold
        ? redThreshold - (blueThreshold + gap)
        : length - (blueThreshold + gap);

    const longArc = blueLength * maxArc * 180 >= 180 ? 1 : 0;

    const blueStartArc = startArc + (blueThreshold + gap) * maxArc;

    const blueArc = blueStartArc + blueLength * maxArc;

    const blueArcBeginX = 90 * Math.cos(blueStartArc * Math.PI);
    const blueArcBeginY = 90 * Math.sin(blueStartArc * Math.PI);

    const blueArcEndX = 90 * Math.cos(blueArc * Math.PI);
    const blueArcEndY = 90 * Math.sin(blueArc * Math.PI);

    return {
      d: `M ${blueArcBeginX},${blueArcBeginY} A 90,90 0 ${longArc} 1 ${blueArcEndX},${blueArcEndY}`,
    };
  });

  const redAnimatedProps = useAnimatedProps(() => {
    const length = lengthSv.value;

    const redLength =
      length < redThreshold + gap ? 0 : length - (redThreshold + gap);

    const longArc = redLength * maxArc * 180 >= 180 ? 1 : 0;

    const redStartArc = startArc + (redThreshold + gap) * maxArc;

    const redArc = redStartArc + redLength * maxArc;

    const redArcBeginX = 90 * Math.cos(redStartArc * Math.PI);
    const redArcBeginY = 90 * Math.sin(redStartArc * Math.PI);

    const redArcEndX = 90 * Math.cos(redArc * Math.PI);
    const redArcEndY = 90 * Math.sin(redArc * Math.PI);

    return {
      d: `M ${redArcBeginX},${redArcBeginY} A 90,90 0 ${longArc} 1 ${redArcEndX},${redArcEndY}`,
    };
  });

  return (
    <View style={[styles.container]}>
      <Svg width="500" height="500" viewBox="-100 -100 200 200">
        <AnimatedPath
          animatedProps={greyAnimatedProps}
          stroke={"#bbb"}
          strokeWidth="4"
        />
        <AnimatedPath
          animatedProps={greenAnimatedProps}
          stroke={"#bfb"}
          strokeWidth="12"
        />
        <AnimatedPath
          animatedProps={blueAnimatedProps}
          stroke={"#bbf"}
          strokeWidth="10"
        />
        <AnimatedPath
          animatedProps={redAnimatedProps}
          stroke={"#fbb"}
          strokeWidth="8"
        />
      </Svg>
    </View>
  );
}

// export function Arc({length, greenThreshold, blueThreshold, redThreshold}: {length: number, greenThreshold: number, blueThreshold: number, redThreshold: number}) {

//   // const arcEndX = 20 + 90 + 90 * Math.sin(length * Math.PI - Math.PI / 2);
//   // console.log(arcEndX);
//   // const arcEndY = 0 + 90 * Math.sin(length * Math.PI);
//   // console.log(arcEndY);

//   const maxArc = 240/180;

//   length = length > 1 ? 1 : length < 0 ? 0 : length;

//   const longArc = length * maxArc >= 90 ? 1 : 0;

//   const startArc = -210/180;

//   const gap = 0.02;

//   const arc = startArc + length * maxArc;
//   // const greyArc = startArc + (length < greenThreshold ? length : greenThreshold) * maxArc;
//   const isGreen = length >= greenThreshold + gap;
//   const isBlue = length >= blueThreshold + gap;
//   const isRed = length >= redThreshold + gap;
//   const greenArc = startArc + (length)

//   const greyArcBeginX = 90 * Math.cos(startArc * Math.PI);
//   const greyArcBeginY = 90 * Math.sin(startArc * Math.PI);

//   const greyArcEndX = 90 * Math.cos((startArc + (length < greenThreshold ? length : greenThreshold) * maxArc)*Math.PI);
//   const greyArcEndY = 90 * Math.sin((startArc + (length < greenThreshold ? length : greenThreshold) * maxArc)*Math.PI);

//   const greenArcBeginX = 90 * Math.cos((startArc + (greenThreshold+gap) * maxArc) * Math.PI);
//   const greenArcBeginY = 90 * Math.sin((startArc + (greenThreshold+gap) * maxArc) * Math.PI);

//   const greenArcEndX = 90 * Math.cos((startArc + (length < blueThreshold ? length : blueThreshold) * maxArc)*Math.PI);
//   const greenArcEndY = 90 * Math.sin((startArc + (length < blueThreshold ? length : blueThreshold) * maxArc)*Math.PI);

//   const blueArcBeginX = 90 * Math.cos((startArc + (blueThreshold+gap) * maxArc) * Math.PI);
//   const blueArcBeginY = 90 * Math.sin((startArc + (blueThreshold+gap) * maxArc) * Math.PI);

//   const blueArcEndX = 90 * Math.cos((startArc + (length < redThreshold ? length : redThreshold) * maxArc)*Math.PI);
//   const blueArcEndY = 90 * Math.sin((startArc + (length < redThreshold ? length : redThreshold) * maxArc)*Math.PI);

//   const redArcBeginX = 90 * Math.cos((startArc + (redThreshold+gap) * maxArc) * Math.PI);
//   const redArcBeginY = 90 * Math.sin((startArc + (redThreshold+gap) * maxArc) * Math.PI);

//   const redArcEndX = 90 * Math.cos(arc * Math.PI);
//   const redArcEndY = 90 * Math.sin(arc * Math.PI);

//   // const greyEndX = 90 * Math.cos((length < greenThreshold ? )

//   // console.log(`M ${arcBeginX},${arcBeginY} A 90,90 0 ${longArc} 1 ${90*Math.cos((startArc+maxArc)*Math.PI)},${90*Math.sin((startArc+maxArc)*Math.PI)}`);

//   // const stroke = length < greenThreshold ? '#999' : length < blueThreshold ? '#bfb' : length < redThreshold ? '#bbf' : '#fbb';

//   return (
//     <View style={styles.container}>
//       <Svg width="400" height="400" viewBox="-100 -100 200 200">
//         {/* <Path
//           d={`M ${arcBeginX},${arcBeginY} A 90,90 0 ${longArc} 1 ${arcEndX},${arcEndY}`}
//           stroke={stroke}
//           strokeWidth="15"
//         /> */}
//                 <Path
//           d={`M ${greyArcBeginX},${greyArcBeginY} A 90,90 0 0 1 ${greyArcEndX},${greyArcEndY}`}
//           stroke={"#bbb"}
//           strokeWidth="5"
//         />
//         {isGreen && (
//           <Path
//             d={`M ${greenArcBeginX},${greenArcBeginY} A 90,90 0 0 1 ${greenArcEndX},${greenArcEndY}`}
//             stroke={"#bfb"}
//             strokeWidth="20"
//           />
//         )}
//         {isBlue &&(
//           <Path
//             d={`M ${blueArcBeginX},${blueArcBeginY} A 90,90 0 0 1 ${blueArcEndX},${blueArcEndY}`}
//             stroke={"#bbf"}
//             strokeWidth="10"
//           />
//         )}
//         {isRed && (
//           <Path
//             d={`M ${redArcBeginX},${redArcBeginY} A 90,90 0 0 1 ${redArcEndX},${redArcEndY}`}
//             stroke={"#fbb"}
//             strokeWidth="10"
//           />
//         )}
//         </Svg>
//     </View>
//   );
// };

const styles = StyleSheet.create({
  container: {
    position: "absolute",
    transform: [{ translateY: -120 }, { translateX: 160 }, { scaleY: -1 }],
  },
});
