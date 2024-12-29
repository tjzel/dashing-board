import {useState, useEffect} from 'react';
import {View, Text, StyleSheet} from 'react-native';

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

  useEffect(() => {
    let job = -1;
    const action = () => {
    fetch('http://localhost:1234/')
      .then((response) => response.json())
      .then((data) => {setJson(data);
        job = requestAnimationFrame(action);
      }).catch((error) => console.log(error));
    };
    action();
    return () => cancelAnimationFrame(job);
  }, []);

  return (
    <View style={styles.container}>
       <Text style={styles.centeredText}>Engine load: {json?.engineLoad}</Text>
       <Text style={styles.centeredText}>Engine RPM: {json?.engineRPM}</Text>
       <Text style={styles.centeredText}>Vehicle speed: {json?.vehicleSpeed}</Text>
       <Text style={styles.centeredText}>Uptime: {json?.uptime}</Text>
       <Text style={styles.centeredText}>Fuel level: {json?.fuelLevel}</Text>
       <Text style={styles.centeredText}>Absolute load: {json?.absoluteLoad}</Text>
       <Text style={styles.centeredText}>Relative throttle position: {json?.relativeThrottlePosition}</Text>
       <Text style={styles.centeredText}>Engine fuel rate: {json?.engineFuelRate}</Text>
    </View>
  );
}

const styles = StyleSheet.create({
  container: {
    flex: 1,
    justifyContent: 'center',
    alignItems: 'center',
  },
  centeredText:{
    textAlign: 'center',
    fontSize: 20,
    color: 'black',
  }
  }
  );
