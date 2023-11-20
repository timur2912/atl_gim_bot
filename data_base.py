import json

class Database:
    def __init__(self, db_file):
        self.db_file = db_file
        self.data = self.load_data()

    def load_data(self):
        try:
            with open(self.db_file, 'r') as file:
                data = json.load(file)
                return data
        except FileNotFoundError:
            return []

    #Метод для сохранения базы данных
    def save_data(self):
        with open(self.db_file, 'w') as file:
            json.dump(self.data, file, indent=4)

    #Метод для пользователя человека в базу данных
    def add_entry(self, telegramid, usernum, array, last_update):
        entry = {
            'telegramid': telegramid,
            'usernum': usernum,
            'array': array,
            'last_update': last_update
        }
        self.data.append(entry)
        self.save_data()

    #Метод для получения всей базы данных
    def get_all_entries(self):
        return self.data


    def get_entry_by_telegramid(self, telegramid):
        for entry in self.data:
            if entry['telegramid'] == telegramid:
                return entry
        return None

    #Метод для обновления базы данных
    def update_entry(self, telegramid, new_array, new_last_update):
        for entry in self.data:
            if entry['telegramid'] == telegramid:
                entry['array'] = new_array
                entry['last_update'] = new_last_update
                self.save_data()
                return
        print(f"Entry with telegramid {telegramid} not found")

    #Метод для удаления пользователя из базы данных
    def delete_entry(self, telegramid):
        for entry in self.data:
            if entry['telegramid'] == telegramid:
                self.data.remove(entry)
                self.save_data()
                return
        print(f"Entry with telegramid {telegramid} not found")

    #Метод, который возвращает список всех telegram id
    def get_all_telegram_ids(self):
        telegram_ids = []
        for entry in self.data:
            telegram_id = entry['telegramid']
            telegram_ids.append(telegram_id)
        return telegram_ids

    def close_connection(self):
        pass  # Не требуется для работы

    #Метод, который возвращает список всех кортежей (telegramid, usernum)
    def get_telegramid_usernum_array(self):
        result = []
        for entry in self.data:
            telegramid = entry['telegramid']
            usernum = entry['usernum']
            result.append((telegramid, usernum))
        return result

db = Database('visits_db.json')
