import 'package:firebase_auth/firebase_auth.dart';
import 'package:flutter/material.dart';
import 'package:firebase_database/firebase_database.dart';
import 'package:Azza/screens//welcome_screen.dart';

class HomeScreen extends StatefulWidget {
  HomeScreen({required this.userEmail});

  final String userEmail;

  @override
  State<HomeScreen> createState() => _HomeScreenState();
}

class _HomeScreenState extends State<HomeScreen> {
  final user = FirebaseAuth.instance.currentUser!;
  final DatabaseReference _sensorDataRef = FirebaseDatabase.instance.ref()
      .child('UsersData').child('m9FKcMggm2gUEYqxRnbTUiiWmro1').child('readings');



  String ultrasonicSensorReading = 'Loading...';
  String flameSensorReading = 'Loading...';
  String timestamp = 'Loading...';

  Future<void> _signOut() async {
    await FirebaseAuth.instance.signOut();
    Navigator.of(context).pushAndRemoveUntil(
      MaterialPageRoute(builder: (context) => WelcomeScreen()),
          (Route<dynamic> route) => false,
    );
  }

  @override
  void initState() {
    super.initState();

    // Listen for changes to the 'Distance' sensor data.
    _sensorDataRef.child('Distance').onValue.listen((event) {
      if (event.snapshot.exists) {
        setState(() {
          ultrasonicSensorReading = event.snapshot.value.toString();
        });
      }
    });

    // Listen for changes to the 'Flame' sensor data and timestamp.
    _sensorDataRef.child('Flame').onValue.listen((event) {
      if (event.snapshot.exists) {
        setState(() {
          flameSensorReading = event.snapshot.value.toString();
          // Assuming the timestamp is stored as a Unix timestamp in Firebase.
          // Convert it to a human-readable format.
          final unixTimestamp = event.snapshot.value;
          final dateTime = unixTimestamp != null ? DateTime.fromMillisecondsSinceEpoch((unixTimestamp as int) * 1000) : DateTime.now();
          timestamp = dateTime.toString();
        });
      }
    });
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: Column(
          crossAxisAlignment: CrossAxisAlignment.start,
          children: [
            Text('Welcome back', style: TextStyle(fontSize: 20)),
            SizedBox(height: 5),
            Text(
              widget.userEmail,
              style: TextStyle(fontSize: 18, fontWeight: FontWeight.bold),
            ),
          ],


        ),

        actions: [
          IconButton(
            onPressed: _signOut,
            icon: Icon(Icons.exit_to_app),
          ),
        ],
      ),


      body: Padding(
        padding: const EdgeInsets.all(16.0),
        child: Column(
          children: [
            Card(
              elevation: 5,
              margin: EdgeInsets.all(10),
              child: Column(
                children: [
                  ListTile(
                    title: Text('Ultrasonic Sensor Reading'),
                    subtitle: Text(ultrasonicSensorReading),
                  ),
                  ListTile(
                    title: Text('Flame Sensor Reading'),
                    subtitle: Text(flameSensorReading),
                  ),
                  ListTile(
                    title: Text('Timestamp'),
                    subtitle: Text(timestamp),
                  ),
                ],
              ),
            ),
            SizedBox(height: 20),
            Card(
              elevation: 5,
              margin: EdgeInsets.all(10),
              child: Column(
                children: [
                  Text(
                    'Car Controller',
                    style: TextStyle(
                      fontSize: 20,
                      fontWeight: FontWeight.bold,
                    ),
                  ),

                  Column(
                    children: [
                      Row(
                        mainAxisAlignment: MainAxisAlignment.spaceAround,
                        children: [
                          IconButton(
                            onPressed: () {
                              // Implement Firebase functionality for moving the car forward.
                              _sensorDataRef.child('Command').set('forward');
                            },
                            icon: Icon(Icons.arrow_upward),
                          ),
                          IconButton(
                            onPressed: () {
                              // Implement Firebase functionality for moving the car backward.
                              _sensorDataRef.child('Command').set('backward');
                            },
                            icon: Icon(Icons.arrow_downward),
                          ),
                          IconButton(
                            onPressed: () {
                              // Implement Firebase functionality for turning the car left.
                              _sensorDataRef.child('Command').set('left');
                            },
                            icon: Icon(Icons.arrow_back),
                          ),
                          IconButton(
                            onPressed: () {
                              // Implement Firebase functionality for turning the car right.
                              _sensorDataRef.child('Command').set('right');
                            },
                            icon: Icon(Icons.arrow_forward),
                          ),
                        ],
                      ),
                      SizedBox(height: 20),
                      IconButton(
                        onPressed: () {
                          // Implement Firebase functionality to stop the car.
                          _sensorDataRef.child('Command').set('stop');
                        },
                        icon: Icon(Icons.stop_circle_outlined),
                      ),
                    ],
                  ),

                ],
              ),
            ),
          ],
        ),
      ),
    );
  }
}
