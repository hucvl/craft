from enum import Enum


class Object:
    class ColorType(Enum):
        BLUE = "blue"
        BROWN = "brown"
        CYAN = "cyan"
        GRAY = "gray"
        GREEN = "green"
        PURPLE = "purple"
        RED = "red"
        YELLOW = "yellow"
        BLACK = "black"

    class GeometryType(Enum):
        SMALL_CUBE = "s_cube"
        BIG_CUBE = "b_cube"
        STANDARD_RECTANGLE = "std_rect"
        SMALL_TRIANGLE = "s_tri"
        BIG_TRIANGLE = "b_tri"
        SMALL_HEXAGON = "s_hex"
        BIG_HEXAGON = "b_hex"
        WALL_PIN = "pin"
        ROPE_UNIT = "rp_unit"
        SMALL_CIRCLE = "s_circle"
        BIG_CIRCLE = "b_circle"
        BIG_RAMP = "b_ramp"
        CUSTOM_RECTANGLE = "cus_rectange"
        CAR_BODY = "car_bd"
        CAR_WHEEL = "car_wh"
        ROD_RECTANGLE = "rod"
        BASKET = "basket"
        LEFT_BOUNDARY = "l_bound"
        RIGHT_BOUNDARY = "r_bound"
        BOTTOM_BOUNDARY = "b_bound"

    KEY_ACTIVE = "active"
    KEY_POSX = "posX"
    KEY_POSY = "posY"
    KEY_BODY_TYPE = "bodyType"  # bodyType=0 --> static, bodyType=1 --> not used, bodyType=2 --> dynamic
    KEY_UNIQUE_ID = "uniqueID"
    KEY_COLOR_TYPE = "colorType"
    KEY_OBJECT_TYPE = "objectType"

    def __init__(self, start_state, end_state):
        self.start_state = start_state
        self.end_state = end_state
        self.isDynamic = self.start_state[Object.KEY_BODY_TYPE] == 0
        self.unique_id = self.start_state[Object.KEY_UNIQUE_ID]
        self.color = Object.ColorType(self.start_state[Object.KEY_COLOR_TYPE])
        self.geometry = Object.GeometryType(self.start_state[Object.KEY_OBJECT_TYPE])

    def is_active_at_start(self):
        return self.start_state[Object.KEY_ACTIVE]

    def is_active_at_end(self):
        return self.end_state[Object.KEY_ACTIVE]

    def pos_x_at_start(self):
        return self.start_state[Object.KEY_POSX]

    def pos_x_at_end(self):
        return self.end_state[Object.KEY_POSX]

    def pos_y_at_start(self):
        return self.start_state[Object.KEY_POSY]

    def pos_y_at_end(self):
        return self.end_state[Object.KEY_POSY]
