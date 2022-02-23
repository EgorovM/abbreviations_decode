from abbr_extraction.abbreviation_extractor import load_extractor

abbr_extractor = load_extractor()

print(abbr_extractor.extract_abbreviations('Я не знаю МКБ или КТ'))