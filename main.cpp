import asyncio
import logging
import time
from aiogram.client.session.aiohttp import AiohttpSession
from aiogram import Bot, Dispatcher, types
from aiogram.filters.command import Command
from aiogram.fsm.storage.memory import MemoryStorage
from sheet import *
from data_base import *
import datetime
import threading


# Включаем логирование, чтобы не пропустить важные сообщения
logging.basicConfig(level=logging.INFO)
# Объект бота
session = AiohttpSession(proxy="http://proxy.server:3128")
bot = Bot(token="ТОКЕН БОТА", session=session, parse_mode='HTML')
# Диспетчер
storage = MemoryStorage()
dp = Dispatcher()

# Хэндлер на команду /start
@dp.message(Command("start"))
async def cmd_start(message: types.Message):
    await message.answer("Выберите в меню комманду /registrate")

# Хэндлер на команду /registrate
@dp.message(Command("registrate"))
async def cmd_registrate(message: types.Message):

    # Отправляем сообщение с просьбой ввести ID
    await message.answer('Пожалуйста, введите номер Вашего электронного студенческого (cм. личный кабинет на mipt.ru:')

    # Создаем обработчик введенного ID
    @dp.message(lambda message: message.text.isdigit())
    async def handle_id(message: types.Message):
        sheet_id = int(message.text)  # Конвертируем ID в целое число

        #Добавления пользователя в базу данных
        db.add_entry(message.from_user.id, str(sheet_id), reader.list_of_workouts(str(sheet_id)), str(datetime.datetime.now()))
        visits_str = '\n'

        #Создаём строку в которой находится список посещений
        for i in db.get_entry_by_telegramid(message.from_user.id)['array']:
            visits_str += i + '\n'

        #Отправка ответа на запрос
        await message.answer('Всего посещений: ' + str(
            len(db.get_entry_by_telegramid(message.from_user.id)['array'])) + '\nВаши посещения: ' + visits_str +
                             '\nПоследнее обновление в базе данных: ' + str(
            db.get_entry_by_telegramid(message.from_user.id)['last_update']))

# Хэндлер на команду /visits
@dp.message(Command("visits"))
async def cmd_visits(message: types.Message):
    # Создаём строку в которой находится список посещений
    visits_str = '\n'
    for i in db.get_entry_by_telegramid(message.from_user.id)['array']:
        visits_str += i + '\n'

    # Отправляем сообщение с ответом на запрос
    await message.answer('Всего посещений: ' + str(len(db.get_entry_by_telegramid(message.from_user.id)['array'])) + '\nВаши посещения: ' + visits_str +
                         '\nПоследнее обновление в базе данных: ' + str(db.get_entry_by_telegramid(message.from_user.id)['last_update']))

#Функция запуска бота
def restart_bot():
    bot.stop_polling()
    dp.stop_polling()


    new_bot = Bot(token='YOUR_BOT_TOKEN')
    new_dp = Dispatcher(new_bot)


    new_bot.start_polling()
    new_dp.start_polling()


async def main():
    thread = threading.Thread(target=update_database_function)
    thread.start()
    await dp.start_polling(bot)

#Json файл с параментрами для сервисного аккаунта Google
credentials_file = 'atlgim-57c246b4d14d.json'
#URL Google таблицы преподавателя
spreadsheet_id = '1gGOlwlh6J4sbkC9ZwWkyVEjl0ukcOOCN8CZBtlFWxn4'
#Объект для работы с Google-таблицей преподавателя
reader = SpreadsheetReader(credentials_file, spreadsheet_id)

def update_database_function():
    #Функция обyовления базы данных, которая работает параллельно ожиданию запросов к боту
    while True:
        usernum_list = db.get_telegramid_usernum_array()
        for u in usernum_list:
            #Необходимо делать задержку между запросами к Google-таблице преподавателя, потому что у DriveApi есть ограничение по количеству запросов в минуту
            time.sleep(5)
            db.update_entry(u[0], reader.list_of_workouts(u[1]), str(datetime.datetime.now()))

            #Вывод в консоль об обновлении данных о пользователе в базе данных бота
            print(u[0], u[1], 'was updated at', str(datetime.datetime.now()))


if __name__ == '__main__':
    asyncio.run(main())
    asyncio.run_forever()

    #Цикл, который перезапускает бота при ошибке
    while True:
        try:
            dp.start_polling()
        except Exception as e:
            print(f'Error: {e}')
            # Restart the bot
            restart_bot()
