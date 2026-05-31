import random

# Thresholds aligned with ESP32智能家居控制逻辑
TEMP_MAX = 28
LIGHT_MIN = 150
TVOC_MAX = 200
CO2_MAX = 1000

RANGE = {
    'temp': (15, 40),
    'light': (0, 500),
    'tvoc': (0, 500),
    'co2': (400, 2000),
}


def run_test(samples=100000):
    result = {
        'fan': 0,
        'fill_light': 0,
        'purifier': 0,
        'temp_above': 0,
        'light_below': 0,
        'air_bad': 0,
    }

    for _ in range(samples):
        temp = random.uniform(*RANGE['temp'])
        light = random.randint(*RANGE['light'])
        tvoc = random.randint(*RANGE['tvoc'])
        co2 = random.randint(*RANGE['co2'])

        if temp > TEMP_MAX:
            result['fan'] += 1
            result['temp_above'] += 1
        if light < LIGHT_MIN:
            result['fill_light'] += 1
            result['light_below'] += 1
        if tvoc > TVOC_MAX or co2 > CO2_MAX:
            result['purifier'] += 1
            result['air_bad'] += 1

    return result


def print_report(report, samples):
    print(f"Monte Carlo 测试样本数: {samples}")
    print(f"风扇启动比例: {report['fan'] / samples:.2%}")
    print(f"补光灯启动比例: {report['fill_light'] / samples:.2%}")
    print(f"净化器启动比例: {report['purifier'] / samples:.2%}")
    print(f"温度超限次数: {report['temp_above']}")
    print(f"光照不足次数: {report['light_below']}")
    print(f"空气质量超标次数: {report['air_bad']}")


if __name__ == '__main__':
    samples = 100000
    report = run_test(samples)
    print_report(report, samples)
