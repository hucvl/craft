class CausalGraph:

    def __init__(self, graph_dict):
        # TODO: Can be optimized, I guess.

        self.__id_to_event = {}  # Mapping integer event IDs to Event objects.
        self.__from_id_to_ids = {}  # Mapping integer event IDs to connected event IDs. Represents edges.

        for event_dict in graph_dict["nodes"]:
            self.__id_to_event[event_dict["id"]] = event_dict
        for edge_dict in graph_dict["edges"]:
            self.__from_id_to_ids[edge_dict["from"]] = edge_dict["to"]

        self.__graph = {}

        for event in self.__id_to_event.values():
            self.__graph[event["id"]] = [self.__id_to_event[event_id] for event_id in self.__from_id_to_ids[event["id"]]]

    @property
    def events(self):
        temp_events = [value for value in self.__id_to_event.values()]
        return sorted(temp_events, key = lambda e: e['step'])

    @property
    def collision_events(self):
        return [event for event in self.events if event["type"] == "Collision"]

    def events_after(self, step_count):
        return [event for event in self.events if event.step > step_count]

    def events_before(self, step_count):
        return [event for event in self.events if event.step < step_count]

    def immediate_outcome_events(self, event):
        return self.__graph[event["id"]]

    def immediate_cause_events(self, event):
        causes = []
        for e in self.events:
            if event in self.immediate_outcome_events(e):
                causes.append(e)
        return causes

    def outcome_events(self, cause):
        all_outcomes = set()
        outcomes = self.immediate_outcome_events(cause)
        all_outcomes.update(outcomes)
        for e in outcomes:
            all_outcomes.update(self.outcome_events(e))
        return list(all_outcomes)

    def cause_events(self, event):
        all_causes = set()
        causes = self.immediate_cause_events(event)
        all_causes.update(causes)
        for e in causes:
            all_causes.update(self.cause_events(e))
        return list(all_causes)

    def event(self, event_id):
        return self.__id_to_event[event_id]
