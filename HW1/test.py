import json


def main():
    with open('./test_cases.json') as data_file:
        data = json.load(data_file)
    print(data['char5']['cases'][0]['input']);
    print(data['char5']['cases'][0]['expected_output']);


main();
