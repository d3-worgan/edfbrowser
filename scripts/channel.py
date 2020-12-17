from lxml import etree
import os
from io import StringIO, BytesIO
from filter_class import FidFilter


class Channel:

    def __init__(self, channel_id, label, factor, voltspercm, screen_offset, polarity, data=None):
        self.id = channel_id
        self.label = label
        self.factor = factor
        self.voltspercm = voltspercm
        self.screen_offset = screen_offset
        self.polarity = polarity
        self.fid_filters = []
        self.data = data
        self.baseline = None

    def update_baseline(self, graph_height, num_channels, graph_top):
        step = graph_height / (num_channels + 1)
        self.baseline = step
        self.baseline += (self.id * int(step)) + (int(graph_top) + int(float(self.screen_offset)))
        self.baseline = int(self.baseline)

    def apply_filters(self):
        pass

    def apply_amplitude(self):
        pass

    def print_channel(self):
        print(f"id: {self.id}")
        print(f"label: {self.label}")
        print(f"factor: {self.factor}")
        print(f"voltspercm: {self.voltspercm}")
        print(f"screen_offset: {self.screen_offset}")
        print(f"polarity: {self.polarity}")
        print(f"filter_cnt: {self.filter_cnt}")



# montage_file_path = os.path.join('E:\\computer_science\\living_lab\\2020TEETACSI\\Analyzer\\REDFBrowser\\EDFbrowser\\scripts\\input_data\montages\\41.mtg')
# print(montage_file_path)
#
# doc = etree.parse(montage_file_path)
# signals = doc.xpath('signalcomposition')
#
# channels = []
# for i, signal in enumerate(signals):
#     idx = i
#     label = signal.xpath('signal/label')
#     factor = signal.xpath('signal/factor')
#     voltpercm = signal.xpath('voltpercm')
#     screen_offset = signal.xpath('screen_offset')
#     polarity = signal.xpath('polarity')
#     filter_cnt = signal.xpath('filter_cnt')
#     channels.append(Channel(idx, label[0].text, factor[0].text, voltpercm[0].text, screen_offset[0].text, polarity[0].text, filter_cnt[0].text))
#     filters = signal.xpath('fidfilter')
#     if len(filters) > 0:
#         for f in filters:
#             ftype = f.xpath('type')
#             freq_1 = f.xpath('frequency')
#             freq_2 = f.xpath('frequency2')
#             ripple = f.xpath('ripple')
#             order = f.xpath('order')
#             model = f.xpath('model')
#             channels[i].fid_filters.append(FidFilter(ftype[0].text, freq_1[0].text, freq_2[0].text, ripple[0].text, order[0].text, model[0].text))
#             channels[i].fid_filters[0].print_filter()

