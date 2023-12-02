import gspread

class SpreadsheetReader:
    def __init__(self, credentials_file, spreadsheet_id):
        self.gc = gspread.service_account(filename=credentials_file)
        self.spreadsheet = self.gc.open_by_key(spreadsheet_id)
        self.worksheet = self.spreadsheet.get_worksheet(0)

    def find_cell(self, search_value):
        cell = self.worksheet.find(search_value)
        return cell

    def get_cell_value(self, cell):
        return self.worksheet.cell(cell.row, cell.col - 8).value

    def get_row_values(self, row):
        return self.worksheet.row_values(row)

    def get_column_values(self, column):
        return self.worksheet.col_values(column)

    def search_value_in_row(self, row, search_value):
        date_list = self.get_row_values(2)
        values_list = self.get_row_values(row)

        result = []
        for i in range(len(values_list)):
            if values_list[i] == search_value:
                result.append(date_list[i])

        return result

    #Метод, который обращается к Google-таблице преподавателя и возвращает список из посещений пользователя
    def list_of_workouts(self, id):
        result = []
        cell = self.find_cell(id)
        #print("Найдено в ячейке R%sC%s" % (cell.row, cell.col))

        val = self.get_cell_value(cell)
        date_list = self.get_row_values(2)
        values_list = self.get_row_values(cell.row)

        for i in range(len(values_list)):
            if values_list[i] == 'п' or values_list[i] == 'б':
                if values_list[i] == 'п':
                    result.append(str(date_list[i]) + ' ' + 'посещение')
                if values_list[i] == 'б':
                        result.append(str(date_list[i]) + ' ' + 'болезнь')
        return result

# Примеры использования методов класса
#print(reader.list_of_workouts("91075"), len(reader.list_of_workouts("91075")))
