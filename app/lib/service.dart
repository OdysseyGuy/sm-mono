import 'package:app/api.dart';
import 'package:app/models.dart';

class UsersService {
  final _api = UsersApi();
  Future<List<User>?> getUsers() async {
    return _api.getUsers();
  }
}