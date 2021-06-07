#!/usr/bin/python3

import json
from loguru import logger
import os
import sys
from glob import glob
from pathlib import Path

from framework.dataset import CRAFTDataset
import vlc
from PyQt5 import QtWidgets, uic
from PyQt5.QtWidgets import QFrame, QListWidget, QLineEdit, QPushButton, QLabel

from framework.utils import FileIO


class Ui(QtWidgets.QMainWindow):

    def __init__(self):
        super(Ui, self).__init__()  # Call the inherited classes __init__ method
        uic.loadUi('dataset_viewer.ui', self)  # Load the .ui file

        self.path = None

        self.btn_load = self.findChild(QPushButton, "loadDatasetButton")
        self.le_dataset_folder = self.findChild(QLineEdit, "datasetFolderLineEdit")
        self.lw_videos = self.findChild(QListWidget, "videosListWidget")
        self.f_video = self.findChild(QFrame, "videoFrame")
        self.lw_questions = self.findChild(QFrame, "questionsListWidget")
        self.btn_replay = self.findChild(QPushButton, "replayButton")
        self.l_video_path = self.findChild(QLabel, "videoPathLabel")

        self.video_index_str = None
        self.le_dataset_folder.setText(self.get_last_dataset_folder())

        self.vlc_instance = vlc.Instance()
        self.vlc_media_player = self.vlc_instance.media_player_new()

        if sys.platform.startswith("linux"):  # for Linux using the X Server
            self.vlc_media_player.set_xwindow(self.f_video.winId())
        elif sys.platform == "win32":  # for Windows
            self.vlc_media_player.set_hwnd(self.f_video.winId())
        elif sys.platform == "darwin":  # for MacOS
            self.vlc_media_player.set_nsobject(int(self.f_video.winId()))

        self.initialize_listeners()

        self.show()  # Show the GUI

    def initialize_listeners(self):
        self.btn_load.clicked.connect(self.load_dataset)
        self.btn_replay.clicked.connect(self.play_video)

    def play_video(self):
        if self.video_index_str is None:
            return

        files = []
        start_dir = Path(f"{self.path}").joinpath("videos")
        pattern = "*.mpg"

        for dir, _, _ in os.walk(start_dir):
            files.extend(glob(os.path.join(dir, pattern)))

        full_path = ""
        for path in files:
            if Path(path).stem == self.video_index_str:
                full_path = path

        logger.debug(f"Full video path: {full_path}")

        self.l_video_path.setText(full_path)

        media = self.vlc_instance.media_new(full_path)
        self.vlc_media_player.set_media(media)
        self.vlc_media_player.play()

        self.lw_questions.clear()
        self.lw_questions.addItems([f"Q: {qa['question']}\nA: {qa['answer']}\nT: {qa['template_id']}\n"
                                    for qa in g_dataset.get_questions_for_video(int(self.video_index_str))])

    def video_item_clicked(self, item):
        logger.debug("Item clicked: " + item.text())
        self.video_index_str = str(item.text()).split(" - ")[0]
        self.play_video()


    def populate_lists(self):
        self.lw_videos.clear()
        self.lw_videos.addItems([f"{key:06d} - SID: {g_dataset.video_index_to_questions_map[key][0]['simulation_id']}" for key in g_dataset.video_index_to_questions_map.keys()])
        self.lw_videos.itemClicked.connect(self.video_item_clicked)


    def get_last_dataset_folder(self):
        last_folder = ""
        if os.path.exists(".state"):
            with open(".state", "r") as state_file:
                last_folder = state_file.read()
        return last_folder

    def load_dataset(self):
        logger.info("Loading dataset...")
        self.path = self.le_dataset_folder.text()

        with open(".state", "w") as state_file:
            state_file.write(self.path)

        global g_dataset
        g_dataset = CRAFTDataset(self.path, FileIO.read_json("../svqa/metadata.json"))
        logger.info(f"Dataset at {self.path} loaded...")
        self.populate_lists()


if __name__ == '__main__':
    app = QtWidgets.QApplication(sys.argv)  # Create an instance of QtWidgets.QApplication
    window = Ui()  # Create an instance of our class
    app.exec_()  # Start the application
