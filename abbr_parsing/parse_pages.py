"""
Автор: Egorov Michil, egorov_michil@mail.ru
Скрипт для парсинга сайта https://cr.minzdrav.gov.ru/
"""

import datetime
from typing import List, Tuple, Dict
import re

from selenium import webdriver
from selenium.webdriver.common.by import By
from selenium.webdriver.support.ui import WebDriverWait
from selenium.webdriver.support.expected_conditions import presence_of_element_located

BASE_URL = 'https://cr.minzdrav.gov.ru/schema/%s'

MAX_UNSUCCESSFULL_COUNT = 50  # количество неудачных попыток, после которого скрипт завершит свою работу
TIMEOUT = 10  # максимальное количество секунд ожидания открытия страницы


def parse(page: str):
    """
    Базовая функция для парсинга страницы
    Можно добавить интересующие функции для определенных элементов
    """
    url = BASE_URL % page
    print('Начался парсинг страницы', url)

    driver.get(url)
    wait.until(
        presence_of_element_located((By.XPATH, '//*[@id="html"]/div[1]/div/h1'))
    )
    html = driver.page_source
    
    return html


if __name__ == '__main__':
    RESULTS = []
    
    start = datetime.datetime.now()
    unsuccessful_count = 0

    with webdriver.Safari() as driver:
        wait = WebDriverWait(driver, TIMEOUT)

        for page_num in range(700):
            page = f'{page_num}_1'

            try:
                result = parse(page)
                RESULTS.append(result)

                unsuccessful_count = 0
                print('Прошло секунд:', (datetime.datetime.now() - start).total_seconds())
            except KeyboardInterrupt:
                break
            except Exception as e:
                unsuccessful_count += 1

                print(
                    f'Что-то пошло не так на странице {page}.'
                    f'\nОшибка: {repr(e)}'
                    f'\nКоличество неудачных попыток подряд: {unsuccessful_count}')

                if MAX_UNSUCCESSFULL_COUNT <= unsuccessful_count:
                    print(
                        f'Количество неудачных попыток перевалило за MAX_UNSUCCESSFULL_COUNT={MAX_UNSUCCESSFULL_COUNT}'
                        f'\nСтраница: {page}'
                    )
                    break

    with open('pages_result.txt', 'w') as f:
        f.write(str(RESULTS))
