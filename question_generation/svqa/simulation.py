from svqa.object import Object
from svqa.causal_graph import CausalGraph


class Simulation:
    def __init__(self, start_scene_dict, end_scene_dict, causal_graph_dict):
        self.objects = []
        for object_state_at_start, object_state_at_end in zip(start_scene_dict, end_scene_dict):
            assert object_state_at_start[Object.KEY_UNIQUE_ID] == object_state_at_end[Object.KEY_UNIQUE_ID]
            self.objects.append(Object(object_state_at_start, object_state_at_end))

        self.causal_graph = CausalGraph(causal_graph_dict)
