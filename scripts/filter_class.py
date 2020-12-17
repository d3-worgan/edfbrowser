

class FidFilter:

    def __init__(self, ftype, frequency_1, frequency_2, ripple, order, model):
        self.ftype = ftype
        self.frequency_1 = frequency_1
        self.frequency_2 = frequency_2
        self.ripple = ripple
        self.order = order
        self.model = model

    def print_filter(self):
        print(f"ftype: {self.ftype}")
        print(f"frequency_1: {self.frequency_1}")
        print(f"frequency_2: {self.frequency_2}")
        print(f"ripple: {self.ripple}")
        print(f"order: {self.order}")
        print(f"model: {self.model}")