import argparse
import os
import random
import subprocess


def parse_args():
    parser = argparse.ArgumentParser()
    parser.add_argument('--dvm', required=True, help="Path to dvm")
    parser.add_argument('--wdir', required=True, help="Path to tests")
    return parser.parse_args()


def generate_grids(max_proc_count, max_dim_count, exact_dims=True):
    if max_dim_count == 1:
        for i in range(1 + int(exact_dims == True),  min(3 + 1, max_proc_count + 1)):
            yield (i,)
    elif max_dim_count > 1:
        for i in range(1,  min(3 + 1, max_proc_count + 1)):
            for grid_suffix in generate_grids(max_proc_count // i, max_dim_count - 1):
                yield (i, *grid_suffix)
    else:
        raise RuntimeError(f'max_proc_count {max_proc_count} < 0')


def compile_test(dvm, fname, result_fname):
    status = subprocess.call(
        f"{dvm} c {fname} -o {result_fname} &> compile-errors.log", shell=True
    )
    if status != 0:
        print(f"Failed to compile `{fname}`. status = {status}")
        subprocess.call("cat compile-errors.log", shell=True)
        exit(1)


def run_test(dvm, fname, grid=()):
    grid_str = " ".join(map(str, grid))
    subprocess.call(
        f"{dvm} run {grid_str} {fname}", shell=True
    )


def run(dvm_path, wdir, max_processes=6):
    subprocess.call("rm -rf control_points", shell=True)
    for dir in os.listdir(os.path.join(wdir, 'C')):
        if dir != 'CHECKPOINT':
            continue
        for test in os.listdir(os.path.join(wdir, 'C', dir)):
            if not test.endswith('.cdv'):
                continue
            full_fname = os.path.join(wdir, 'C', dir, test)
            output_name = full_fname.rsplit('.', 1)[0]
            test_dims = list(map(int, filter(lambda x: x.isdigit(), test)))
            compile_test(dvm_path, full_fname, output_name)
            grids = list(generate_grids(max_processes, max(test_dims)))
            # print(grids)
            for grid in random.sample(grids, min(2, len(grids))):
                print(f"Running {test} on grid `{grid}`")
                run_test(dvm_path, output_name, grid=grid)


if __name__ == '__main__':
    args = parse_args()
    run(args.dvm, args.wdir)


# FAILES
# Running acr44.cdv on grid `(1, 1, 1, 4)`
# Running owncal41.cdv on grid `(2, 1, 1, 2)`