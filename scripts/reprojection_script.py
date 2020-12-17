import numpy
import os
import json
import argparse
import re
import cv2
import time
import lxml.etree
from lxml import etree

from filter_class import FidFilter
from channel import Channel
from edflib.edfreader import EDFreader


class UITracker:

    def __init__(self, input_directory, output_directory=None):
        
        self.edf_file_path = None
        self.edf = None

        # Setup directory structures
        self.input_directory = input_directory
        if output_directory is None:
            self.output_directory = os.path.join(os.getcwd(), 'Results')
            print(f"Putting results in {self.output_directory}")
        else:
            self.output_directory = output_directory
        if os.path.exists(self.output_directory):
            files = next(os.walk(self.output_directory))[2]
            if len(files) > 0:
                proceed = input(f"Do you want to overwrite the data in {self.output_directory}? y/n")
                if proceed == 'y':
                    files = next(os.walk(self.output_directory))[2]
                    for f in files:
                        os.remove(os.path.join(self.output_directory, f))
                    time.sleep(1)
                    os.rmdir(self.output_directory)
                    os.mkdir(self.output_directory)
                else:
                    raise FileExistsError
        else:
            os.mkdir(self.output_directory)
        assert os.path.exists(self.input_directory), "Couldnt find the input directory"
        assert os.path.exists(self.output_directory), "Something wen wrong creating the results directory"
        
        self.log_file_path = os.path.join(input_directory, 'ui_log.txt')
        
        self.montages_directory = os.path.join(input_directory, 'montages')
        assert os.path.exists(self.montages_directory), "Couldnt find the montages directory"
        self.montage_file_name = None
        self.montage_file_path = None

        self.screenshots_directory = os.path.join(input_directory, 'screenshots')
        assert os.path.exists(self.screenshots_directory), "Couldnt find the screenshots directory"
        self.screenshot_file_name = None
        self.screenshot_file_path = None
        self.screenshot = None
        self.screenshots = next(os.walk(self.input_directory))[2]
        assert len(self.screenshots) > 0, f"There were no screenshots in {self.screenshots}"
        
        self.reprojection_file_path = None
        
        # Current UI state
        self.log_id = None
        self.last_event = None
        self.current_timestamp = None
        self.graph_width = None
        self.graph_height = None
        self.graph_top_left = None
        self.graph_top_right = None
        self.graph_bottom_left = None
        self.graph_bottom_right = None
        self.time_position = None
        self.time_scale = None
        self.num_channels = None
        self.channels = []

        self.line_color = (0, 255, 0)
        self.line_width = 3

    def simulate_reprojections(self):
        """
        Go through each entry and screenshot in the log and redraw the 'reprojected' information
        recorded in the log.
        """
        log = open(self.log_file_path)
        while True:

            # Each line of the log file
            line = log.readline()
            if not line :
                break
            line.strip()

            # Get the log info
            entry = json.loads(line)
            event = entry['event']
            data = entry['data']
            self.log_id = entry['id']
            self.current_timestamp = entry['timestamp']

            # Update the state according the recorded events
            if event == 'FILE_OPENED':
                self.edf_file_path = data['edf_path']
                self.time_position = data['time']
                self.time_scale = data['time_scale']
                self.montage_file_name = data['montage_file']
                self.load_channels_from_montage()
                self.graph_width = data['graph_dimensions'][0]
                self.graph_height = data['graph_dimensions'][1]
                graph_box = data['graph_box']
                self.graph_top_left = (graph_box['top_left'][0], graph_box['top_left'][1])
                self.graph_top_right = (graph_box['top_right'][0], graph_box['top_right'][1])
                self.graph_bottom_left = (graph_box['bottom_left'][0], graph_box['bottom_left'][1])
                self.graph_bottom_right = (graph_box['bottom_right'][0], graph_box['bottom_right'][1])
                self.last_event = 'FILE_OPENED'
                print("Reading edf file")
                if os.path.exists(self.edf_file_path):
                    self.edf = EDFreader(self.edf_file_path)
                else:
                    user_input = input(f"The path to the edf file ({self.edf_file_path}) is invalid. Please specify the path"
                                       f"to the edf file. ")
                    assert os.path.exists(user_input), f"The path {user_input} does not lead to a valid edf file."
                    self.edf = EDFreader(self.edf_file_path)
            elif event == 'FILE_CLOSED':
                break
            elif event == 'MONTAGE_CHANGED':
                self.montage_file_name = data['montage_file']
                self.load_channels_from_montage()
                self.last_event = 'MONTAGE_CHANGED'
            elif event == 'CHANNELS_CHANGED':
                self.montage_file_name = data['montage_file']
                self.load_channels_from_montage()
                self.last_event = 'CHANNELS_CHANGED'
            elif event == 'FILTER_CHANGED':
                self.montage_file_name = data['montage_file']
                self.load_channels_from_montage()
                self.last_event = 'FILTER_CHANGED'
            elif event == 'AMPLITUDE_CHANGED':
                self.montage_file_name = data['montage_file']
                self.load_channels_from_montage()
            elif event == 'TIMESCALE_CHANGED':
                self.time_scale = data['time_scale']
                self.last_event = 'TIMESCALE_CHANGED'
            elif event == 'TIME_POSITION_CHANGED':
                self.time_position = data['time']
                self.last_event = 'TIME_POSITION_CHANGED'
            elif event == 'VERTICAL_CHANGED':
                self.montage_file_name = data['montage_file']
                self.load_channels_from_montage()
                self.last_event = 'VERTICAL_CHANGED'
            elif event == 'ZOOM_CHANGED':
                self.montage_file_name = data['montage_file']
                self.load_channels_from_montage()
                self.time_scale = data['time_scale']
                self.time_position = data['time']
                self.last_event = 'ZOOM_CHANGED'
            elif entry['event'] == 'WINDOW_MOVED':
                graph_box = data['graph_box']
                self.graph_top_left = (graph_box['top_left'][0], graph_box['top_left'][1])
                self.graph_top_right = (graph_box['top_right'][0], graph_box['top_right'][1])
                self.graph_bottom_left = (graph_box['bottom_left'][0], graph_box['bottom_left'][1])
                self.graph_bottom_right = (graph_box['bottom_right'][0], graph_box['bottom_right'][1])
                self.last_event = 'WINDOW_MOVED'
            elif entry['event'] == 'WINDOW_RESIZED':
                self.graph_width = data['graph_dimensions'][0]
                self.graph_height = data['graph_dimensions'][1]
                graph_box = data['graph_box']
                self.graph_top_left = (graph_box['top_left'][0], graph_box['top_left'][1])
                self.graph_top_right = (graph_box['top_right'][0], graph_box['top_right'][1])
                self.graph_bottom_left = (graph_box['bottom_left'][0], graph_box['bottom_left'][1])
                self.graph_bottom_right = (graph_box['bottom_right'][0], graph_box['bottom_right'][1])
                self.last_event = 'WINDOW_RESIZED'
            else:
                print("Invalid event!")
            
            # Get the corresponding screenshot and draw the bounding box
            self.screenshot_file_name = f"{int(self.log_id) + 1}.png"  # +1 because screenshots are 1 event behind atm
            self.screenshot_file_path = os.path.join(self.screenshots_directory, self.screenshot_file_name)
            assert os.path.exists(self.screenshot_file_path), f"{self.screenshot_file_path} doesnt exist"
            self.screenshot = cv2.imread(self.screenshot_file_path)
            self.screenshot = self.draw_graph_bbox(self.screenshot)
            self.screenshot = self.draw_log_info(self.screenshot)
            if self.montage_file_name is not None:
                self.screenshot = self.draw_channel_baselines(self.screenshot)
            self.reprojection_file_path = os.path.join(self.output_directory, self.screenshot_file_name)
            cv2.imwrite(self.reprojection_file_path, self.screenshot)

        log.close()

    def load_channels_from_montage(self):

        # Update the current montage file path
        if self.montage_file_name is not None:
            if '.mtg' not in self.montage_file_name:
                self.montage_file_name = f"{self.montage_file_name}.mtg"
            self.montage_file_path = os.path.join(self.montages_directory, self.montage_file_name)

            print(self.montages_directory)
            print(self.montage_file_name)
            print(self.montage_file_path)

            assert os.path.exists(self.montage_file_path)
            # Read in the channels from the mtg file
            doc = etree.parse(self.montage_file_path)
            signals = doc.xpath('signalcomposition')
            self.channels.clear()
            for i, signal in enumerate(signals):
                # Channel info
                idx = i
                label = signal.xpath('signal/label')
                factor = signal.xpath('signal/factor')
                voltpercm = signal.xpath('voltpercm')
                screen_offset = signal.xpath('screen_offset')
                polarity = signal.xpath('polarity')
                filter_cnt = signal.xpath('filter_cnt')
                self.channels.append(Channel(idx, label[0].text, factor[0].text, voltpercm[0].text, screen_offset[0].text, polarity[0].text, filter_cnt[0].text))

                # Channel filters
                filters = signal.xpath('fidfilter')
                if len(filters) > 0:
                    for f in filters:
                        ftype = f.xpath('type')
                        freq_1 = f.xpath('frequency')
                        freq_2 = f.xpath('frequency2')
                        ripple = f.xpath('ripple')
                        order = f.xpath('order')
                        model = f.xpath('model')
                        self.channels[i].fid_filters.append(FidFilter(ftype[0].text, freq_1[0].text, freq_2[0].text, ripple[0].text, order[0].text, model[0].text))

                # Channel data
                # start = self.edf.getSampleFrequency(i) * self.time_position
                # num_samples_to_read = self.edf.getSampleFrequency(i) * self.time_scale
                # assert start == self.edf.fseek(i, start, 'EDFSEEK_SET')
                # self.channels[i].data = numpy.empty(num_samples_to_read, dtype=numpy.int32)
                # assert num_samples_to_read == self.edf.readSamples(idx, self.channels[i].data, num_samples_to_read)

                # TODO: Apply filters to data
                # TODO: Apply amplitude to data

                # Update the channel baseline
                self.channels[i].update_baseline(self.graph_height, len(self.channels), self.graph_top_left[1])

    def update_channel_data(self):
        for i, channel in enumerate(self.channels):
            start = self.edf.getSampleFrequency(i) * self.time_position
            num_samples_to_read = self.edf.getSampleFrequency(i) * self.time_scale
            assert start == self.edf.fseek(i, start, 'EDFSEEK_SET')
            self.channels[i].data = numpy.empty(num_samples_to_read, dtype=numpy.float_)
            assert num_samples_to_read == self.edf.readSamples(i, self.channels[i].data, num_samples_to_read)

    def draw_graph_bbox(self, image):
        image = cv2.line(image, self.graph_top_left, self.graph_top_right, self.line_color, self.line_width)
        image = cv2.line(image, self.graph_bottom_left, self.graph_bottom_right, self.line_color, self.line_width)
        image = cv2.line(image, self.graph_top_left, self.graph_bottom_left, self.line_color, self.line_width)
        image = cv2.line(image, self.graph_top_right, self.graph_bottom_right, self.line_color, self.line_width)
        return image

    def draw_log_info(self, image):
        image = cv2.putText(image, f"id: {self.log_id}", (20, 100), cv2.FONT_HERSHEY_SIMPLEX, 1, self.line_color, 2, cv2.LINE_AA)
        image = cv2.putText(image, f"time: {self.time_position}", (20,130), cv2.FONT_HERSHEY_SIMPLEX, 1, self.line_color, 2, cv2.LINE_AA)
        image = cv2.putText(image, f"timescale: {self.time_scale}", (20,160), cv2.FONT_HERSHEY_SIMPLEX, 1, self.line_color, 2, cv2.LINE_AA)
        image = cv2.putText(image, f"montage: {self.montage_file_name}", (20, 190), cv2.FONT_HERSHEY_SIMPLEX, 1, self.line_color, 2, cv2.LINE_AA)
        image = cv2.putText(image, f"width: {self.graph_width}", (20,220), cv2.FONT_HERSHEY_SIMPLEX, 1, self.line_color, 2, cv2.LINE_AA)
        image = cv2.putText(image, f"height: {self.graph_height}", (20,250), cv2.FONT_HERSHEY_SIMPLEX, 1, self.line_color, 2, cv2.LINE_AA)
        image = cv2.putText(image, f"last event: {self.last_event}", (20,280), cv2.FONT_HERSHEY_SIMPLEX, 1, self.line_color, 2, cv2.LINE_AA)
        return image
    
    def draw_channel_baselines(self, image):
        graph_left = self.graph_top_left[0]
        graph_right = self.graph_top_right[0]
        graph_top = self.graph_top_left[1]
        for i, channel in enumerate(self.channels):
            channel.update_baseline(self.graph_height, len(self.channels), graph_top)
            image = cv2.line(image, (graph_left, channel.baseline), (graph_right, channel.baseline), self.line_color, self.line_width)

        # print(f"Num channels {len(self.channels)}")
        # vertical_distance = (self.graph_height - 7) / (self.num_channels + 1)
        # for i, channel in enumerate(self.channels):
        #     baseline = vertical_distance * (i + 1)
        #     baseline += self.graph_top_left[1]
        #     channel.baseline = baseline
        #     image = cv2.line(image, (graph_left, int(baseline)), (graph_right, int(baseline)), self.line_color, self.line_width)

        return image

    

def parse_arguments():
    parser = argparse.ArgumentParser()
    parser.add_argument("--input", required=True, help="Specify a directory containing the ui.log and corresponding images to draw on")
    return parser.parse_args()


if __name__ == "__main__":
    args = parse_arguments()
    ui_tracker = UITracker(args.input)
    ui_tracker.simulate_reprojections()
