import 'dart:developer';

import 'package:http/http.dart' as http;
import 'package:app/constants.dart';
import 'package:app/models.dart';

class UsersApi {
  Future<List<User>?> getUsers() async {
    try {
      var url = Uri.parse(ApiConstants.baseUrl+ApiConstants.usersEndpoint);
      var response = await http.get(url);
      if (response.statusCode == 200) {
        List<User> users = [];
        return users;
      }
    } catch(e) {
      log(e.toString());
    }
  }
}
