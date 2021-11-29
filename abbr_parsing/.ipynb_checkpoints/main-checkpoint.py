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

# Пути до основных элементов в странице
HEADER_FIELDS = {
    'name': (By.XPATH, '//*[@id="html"]/div[1]/div/h1'),
    'mkb_code': (By.XPATH, '//*[@id="mkb"]'),
    'start_year': (By.XPATH, '//*[@id="html"]/div[1]/div/div[2]/div[2]/b'),
    'age_category': (By.XPATH, '//*[@id="html"]/div[1]/div/div[2]/div[3]/b'),
    'end_year': (By.XPATH, '//*[@id="html"]/div[1]/div/div[3]/div[1]/b'),
    'id': (By.XPATH, '//*[@id="html"]/div[1]/div/div[3]/div[2]/b'),
}  


def parse_headers() -> Dict[str, str]:
    """
    :return: словарь справочной информации
    """
    values = {}

    for field, params in HEADER_FIELDS.items():
        first_result = wait.until(presence_of_element_located(params))
        values[field] = first_result.get_attribute("textContent")

    return values


def parse_abbreviations() -> Tuple[List[Tuple[str, str]], str]:
    """
    :return: список аббревиатур вида [(abbr1, term1), (abbr2, term2)...] и на всякий весь текст аббревиатур
    """
    abbreviations_div = driver.find_element_by_xpath('//h1[@id="doc_abbreviation"]/following-sibling::div')
    text = abbreviations_div.get_attribute('textContent')

    abbreviations = []

    for str_pair in text.split('\n'):
        pair = re.split(r'[–|—|-|‒]', str_pair, maxsplit=1)

        if len(pair) != 2:
            print('Что то пошло не так...', str_pair)
            continue

        pair: Tuple[str, str] = tuple(map(lambda x: x.strip(), pair))
        abbreviations.append(pair)

    return abbreviations, text


def parse(page: str):
    """
    Базовая функция для парсинга страницы
    Можно добавить интересующие функции для определенных элементов
    """
    url = BASE_URL % page
    print('Начался парсинг страницы', url)

    driver.get(url)

    headers = parse_headers()
    abbreviations, abbreviations_text = parse_abbreviations()
    
    return headers, abbreviations, abbreviations_text


def fill_login():
    inputElement = driver.find_element_by_id("a1")
    inputElement.send_keys('1')
    inputElement.send_keys(Keys.ENTER)
    inputElement.submit()
    

if __name__ == '__main__':
    RESULTS = []
    
    start = datetime.datetime.now()
    unsuccessful_count = 0

    with webdriver.Safari() as driver:
        wait = WebDriverWait(driver, TIMEOUT)

        for page_num in range(430, 700):
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

    with open('result430-700.txt', 'w') as f:
        f.write(str(RESULTS))
