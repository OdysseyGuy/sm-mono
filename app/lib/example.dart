import 'package:flutter/material.dart';
import 'dart:developer' as developer;
import 'package:web_socket_channel/web_socket_channel.dart';

void main() {
  runApp(const MyApp());
}

class MyApp extends StatelessWidget {
  const MyApp({super.key});

  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      title: 'Voltage Client',
      theme: ThemeData(
        primarySwatch: Colors.blue,
      ),
      home: const MyHomePage(),
    );
  }
}

class MyHomePage extends StatefulWidget {
  const MyHomePage({super.key});

  @override
  State<MyHomePage> createState() => _MyHomePageState();
}

class _MyHomePageState extends State<MyHomePage> {
  final TextEditingController _controller = TextEditingController();
  WebSocketChannel? _channel;
  String? _voltage;

  @override
  void initState() {
    super.initState();
    _connectToWebSocket();
  }

  void _connectToWebSocket() {
    _channel = WebSocketChannel.connect(Uri.parse('ws://localhost:50215'));
    _channel!.stream.listen((message) {
      setState(() {
        _voltage = message;
      });
    }, onError: (error) {
      developer.log(error);
      setState(() {
        _voltage = 'Error: $error';
      });
    });
  }

  void _getVoltage() {
    if (_channel != null && _controller.text.isNotEmpty) {
      _channel!.sink.add(_controller.text);
    }
  }

  @override
  void dispose() {
    _channel?.sink.close();
    _controller.dispose();
    super.dispose();
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: const Text('Voltage Client'),
      ),
      body: Padding(
        padding: const EdgeInsets.all(16.0),
        child: Column(
          children: <Widget>[
            TextField(
              controller: _controller,
              decoration: const InputDecoration(labelText: 'Client ID'),
            ),
            const SizedBox(height: 20),
            ElevatedButton(
              onPressed: _getVoltage,
              child: const Text('Get Voltage'),
            ),
            const SizedBox(height: 20),
            if (_voltage != null)
              Text(
                'Voltage: $_voltage',
                style: const TextStyle(fontSize: 24),
              ),
          ],
        ),
      ),
    );
  }
}
