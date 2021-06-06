import glob
import time

import os
import numpy as np
import ujson

from multiprocessing import Process
from threading import Thread
from typing import List

from loguru import logger

from shutil import copyfile, move


class DictUtils:

    @staticmethod
    def scrub(obj, key_to_be_removed):
        if isinstance(obj, dict):
            # the call to `list` is useless for py2 but makes
            # the code py2/py3 compatible
            for key in list(obj.keys()):
                if key == key_to_be_removed:
                    obj.pop(key, None)
                else:
                    DictUtils.scrub(obj[key], key_to_be_removed)
        elif isinstance(obj, list):
            for i in reversed(range(len(obj))):
                if obj[i] == key_to_be_removed:
                    del obj[i]
                else:
                    DictUtils.scrub(obj[i], key_to_be_removed)

        else:
            # neither a dict nor a list, do nothing
            pass


class FileIO:

    @staticmethod
    def write_to_file(file_path, content):
        with open(file_path, "w") as f:
            f.write(content)
            f.close()

    @staticmethod
    def read_json(file_path):
        with open(file_path, "rb") as f:
            json_obj = ujson.load(f)
            f.close()
        return json_obj

    @staticmethod
    def write_json(json_obj, file_path):
        with open(file_path, "w") as f:
            ujson.dump(json_obj, f, escape_forward_slashes=False)
            f.close()

    @staticmethod
    def copy(from_path, to_path):
        copyfile(from_path, to_path)

    @staticmethod
    def move(from_path, to_path):
        os.rename(from_path, to_path)

    @staticmethod
    def delete_files(folder_path, *wildcards):
        files_to_be_removed = []
        for w in wildcards:
            files_to_be_removed.extend([os.path.abspath(c) for c in glob.glob(f"{folder_path}/{w}")])

        for path in files_to_be_removed:
            os.remove(path) if os.path.exists(path) else None


class Funnel:
    def __init__(self, lst: list):
        self.__list = list(lst)

    def get_result(self) -> list:
        return self.__list

    def filter(self, predicate):
        self.__list = list(filter(predicate, self.__list))
        return self


class ParallelProcessor(object):
    """
    To process the functions in parallel
    """

    def __init__(self, jobs: list, args: list):
        """
        """
        self.jobs = jobs
        self.args = args
        self.processes: List[Process] = []

    def fork_processes(self):
        """
        Creates the process objects for given function delegates
        """
        for i in range(len(self.jobs)):
            job = self.jobs[i]
            job_args = self.args[i]
            proc = Process(target=job, args=job_args)
            self.processes.append(proc)

    def start_all(self):
        """
        Starts the functions process all together.
        """
        for proc in self.processes:
            proc.start()

    def join_all(self):
        """
        Waits until all the functions executed.
        """
        for proc in self.processes:
            proc.join()


class ParallelWorker(object):

    def __init__(self, jobs: list, args: list, concurrent_process_count: int):
        self.jobs = jobs
        self.args = args
        self.concurrent_process_count = concurrent_process_count
        self.__times = np.array([])

    def execute_all(self):
        job_count = len(self.jobs)
        full_iterations = job_count // self.concurrent_process_count

        for i in range(full_iterations + 1):
            if i >= job_count: continue

            t1 = time.time()

            start_index = i * self.concurrent_process_count
            end_index = min(start_index + self.concurrent_process_count, job_count)

            parallel_processor = ParallelProcessor(self.jobs[start_index:end_index],
                                                   self.args[start_index:end_index])

            logger.info(f"Forking processes [{start_index}-{end_index - 1}] into parallel")
            parallel_processor.fork_processes()

            logger.info(f"Executing processes in parallel")
            parallel_processor.start_all()

            logger.info(f"Waiting for all concurrent processes to finish")
            parallel_processor.join_all()

            self.__update_clock((time.time() - t1) / self.concurrent_process_count,
                                job_count,
                                i + self.concurrent_process_count)

    def __update_clock(self, diff, total_runs: int, current: int):
        times = np.append(self.__times, diff)
        logger.info(f"Approximately {round((np.mean(times) * (total_runs - current - 1)) / 60, 2)} minutes remaining")



class MultithreadedProcessor(object):
    """
    To process the functions in parallel
    """

    def __init__(self, jobs: list, args: list):
        """
        """
        self.jobs = jobs
        self.args = args
        self.threads: List[Thread] = []

    def fork_processes(self):
        """
        Creates the process objects for given function delegates
        """
        for i in range(len(self.jobs)):
            job = self.jobs[i]
            job_args = self.args[i]
            t = Thread(target=job, args=job_args)
            self.threads.append(t)

    def start_all(self):
        """
        Starts the functions process all together.
        """
        for t in self.threads:
            t.start()

    def join_all(self):
        """
        Waits until all the functions executed.
        """
        for t in self.threads:
            t.join()


class MultithreadedWorker(object):

    def __init__(self, jobs: list, args: list, concurrent_process_count: int):
        self.jobs = jobs
        self.args = args
        self.concurrent_process_count = concurrent_process_count
        self.__times = np.array([])

    def execute_all(self):
        job_count = len(self.jobs)
        full_iterations = job_count // self.concurrent_process_count

        for i in range(full_iterations + 1):
            if i >= job_count: continue

            t1 = time.time()

            start_index = i * self.concurrent_process_count
            end_index = min(start_index + self.concurrent_process_count, job_count)

            multithreaded_processor = MultithreadedProcessor(self.jobs[start_index:end_index],
                                                   self.args[start_index:end_index])

            logger.info(f"Forking threads for [{start_index}-{end_index - 1}]")
            multithreaded_processor.fork_processes()

            logger.info(f"Executing threads concurrently")
            multithreaded_processor.start_all()

            logger.info(f"Waiting for all concurrent threads to finish")
            multithreaded_processor.join_all()

            self.__update_clock((time.time() - t1) / self.concurrent_process_count,
                                job_count,
                                i + self.concurrent_process_count)

    def __update_clock(self, diff, total_runs: int, current: int):
        times = np.append(self.__times, diff)
        logger.info(f"Approximately {round((np.mean(times) * (total_runs - current - 1)) / 60, 2)} minutes remaining")



def job(index):
    print(index)


if __name__ == '__main__':
    jobs = []
    args = []

    for i in range(100):
        jobs.append(job)
        args.append([i + 1])

    parallel_worker = ParallelWorker(jobs, args, 16)

    parallel_worker.execute_all()
