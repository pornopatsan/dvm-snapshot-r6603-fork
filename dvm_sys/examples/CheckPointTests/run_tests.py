import os
import re
import logging
import random
import argparse
import multiprocessing as mp

FORMAT = '%(asctime)-15s %(levelname)-8s %(message)s'
logging.basicConfig(format=FORMAT, level=logging.DEBUG)

THIS_FILENAME = str(__file__).split('/')[-1]
TESTS_DIR = os.path.dirname(__file__)
USER_DIR = TESTS_DIR.replace('examples/CheckPointTests', 'user')
TESTS_WORKDIR = USER_DIR + '/tests'
TESTS_DATA = TESTS_WORKDIR + '/data'


def parse_args():
    args = argparse.ArgumentParser()
    args.add_argument('--compile-only', action='store_true')
    args.add_argument('--test-regex', default=r'test.*', help='regexp to filter test names and run a sample of tests')
    return args.parse_args()


def get_grid_list(filename, size_=3, min_=1, max_=2):
    nd = re.search(r'_(\d)d', filename)
    if nd:
        n = int(nd.group(1))
        return list(list(random.randint(min_, max_) for _ in range(n)) for _ in range(size_))
    return [[]]


def run_test(task, grid_list):
    for grid in grid_list:
        grid_str = ' '.join(str(d) for d in grid)
        command = f'{USER_DIR}/dvm run {grid_str} {TESTS_WORKDIR}/{task}'
        logging.info(f'Running command:\t{command}')
        status = os.system(command)
        yield bool(status) == bool('_fail' in task)


def main():
    args = parse_args()
    logging.info('Start unit-testing')
    logging.info(f'System cpu cores count = {mp.cpu_count()}')

    files_set = set(os.listdir(TESTS_DIR))
    files_set = set(filter(lambda x: x.endswith('.c') or x.endswith('.cdv'), files_set))
    files_set = set(filter(lambda x: re.match(args.test_regex, x), files_set))
    logging.info(f'Found these tests: {files_set}')

    os.system(f'rm -rf control_points')
    os.system(f'rm -rf {TESTS_DATA}')
    os.system(f'mkdir {TESTS_DATA}')

    for test_file in files_set:
        logging.info(f'Compiling file:\t{USER_DIR}/dvm c {TESTS_DIR}/{test_file}')
        status = os.system(f'{USER_DIR}/dvm c {TESTS_DIR}/{test_file} &> {TESTS_DATA}/compile_status')
        logging.debug(f'Status\t{"OK" if status == 0 else status}')
        if status != 0:
            logging.error(f'Error compiling {test_file}, information is below')
            os.system(f'cat {TESTS_DATA}/compile_status')
            raise RuntimeError(f'Error compiling {test_file}')

    if args.compile_only:
        logging.info('Finished')
        return

    ok_tests, failed_tests, total_tests = 0, 0, len(files_set)
    for test_file in sorted(files_set):
        all_passed = True
        for passed in run_test(test_file.split('.')[0], get_grid_list(test_file)):
            if passed:
                logging.info('OK')
            else:
                logging.error('FAILED')
                all_passed = False
        if all_passed:
            ok_tests += 1
        else:
            failed_tests += 1

    logging.info(f'PASSED:\t{ok_tests}/{total_tests}')
    logging.info(f'FAILED:\t{failed_tests}/{total_tests}')


if __name__ == '__main__':
    main()
