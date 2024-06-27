import 'dart:convert';

import 'package:flutter/material.dart';
import 'package:flutter_secure_storage/flutter_secure_storage.dart';
import 'package:http/http.dart' as http;

class LoginScreen extends StatefulWidget {
  const LoginScreen({super.key});

  @override
  State<LoginScreen> createState() => _LoginScreenState();
}

class _LoginScreenState extends State<LoginScreen> {
  final TextEditingController _consumerIdController = TextEditingController();
  final TextEditingController _meterIdController = TextEditingController();
  final storage = const FlutterSecureStorage();
  String _message = '';

  Future<void> _login() async {
    final response = await http.post(
      Uri.parse('http://localhost:3000/login'),
      headers: <String, String>{
        'Content-Type': 'application/json; charset=UTF-8',
      },
      body: jsonEncode(<String, String>{
        'consumerId': _consumerIdController.text,
        'meterId': _meterIdController.text,
      }),
    );

    if (response.statusCode == 200) {
      final data = jsonDecode(response.body);
      final token = data['token'];
      await storage.write(key: 'smjwt', value: token);
      Navigator.push(context, route)
    } else {
      setState(() {
        _message = 'Failed to login';
      });
    }

    
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(title: const Text('Login')),
      body: Padding(
        padding: const EdgeInsets.all(16.0),
        child: Column(
          children: [
            TextField(
              controller: _consumerIdController,
              decoration: const InputDecoration(labelText: 'Consumer ID'),
            ),
            TextField(
              controller: _meterIdController,
              decoration: const InputDecoration(labelText: 'Meter ID'),
            ),
            ElevatedButton(
              onPressed: _login,
              child: const Text('Login'),
            ),
            Text(_message),
          ],
        ),
      ),
    );
  }

  @override
  void dispose() {
    super.dispose();
  }
}
