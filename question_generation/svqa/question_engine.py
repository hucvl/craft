# Copyright 2017-present, Facebook, Inc.
# All rights reserved.
#
# This source code is licensed under the BSD-style license found in the
# LICENSE file in the root directory of this source tree. An additional grant
# of patent rights can be found in the PATENTS file in the same directory.

import json, os, math
from collections import defaultdict

from svqa.causal_graph import CausalGraph

"""
Utilities for working with function program representations of questions.

Some of the metadata about what question node types are available etc are stored
in a JSON metadata file.
"""

#Helpers

def object_with_unique_id(scene_struct, unique_object_id):
    objs = [o for o in scene_struct['objects'] if o['uniqueID']==unique_object_id]
    try:
        assert(len(objs)==1)
    except:
        val = 2
    return objs[0]

def is_ground(scene_structs, unique_object_id):
    return unique_object_id == get_ground_unique_id(scene_structs)

def is_basket(scene_structs, unique_object_id):
    return unique_object_id == get_basket_unique_id(scene_structs)

def get_ground_unique_id(scene_structs):
    objs = [o for o in scene_structs[0]['objects'] if o['shape'] == 'ground']
    assert (len(objs) == 1)
    return objs[0]['uniqueID']

def get_basket_unique_id(scene_structs):
    objs = [o for o in scene_structs[0]['objects'] if o['shape'] == 'basket']
    assert (len(objs) == 1)
    return objs[0]['uniqueID']

def is_object_moving(obj):
    eps = 0.001
    if obj['bodyType']!= 0 and (math.fabs(obj['2dLinearVelocity'][0])>eps or math.fabs(obj['2dLinearVelocity'][1])>eps): #0 is a static body
        return True
    return False

def is_object_dynamic(obj):
    return obj['bodyType'] != 0

def start_scene_handler(variations_outputs, scene_structs, causal_graph, inputs, side_inputs):
    # Just return all objects in the start scene
    return [o['uniqueID'] for o in scene_structs[0]['objects']]

def end_scene_handler(variations_outputs, scene_structs, causal_graph, inputs, side_inputs):
    # Just return all objects in the end scene
    return [o['uniqueID'] for o in scene_structs[-1]['objects']]

def make_filter_handler(attribute):
    def filter_handler(variations_outputs, scene_structs, causal_graph, inputs, side_inputs):
        assert len(inputs) == 1
        assert len(side_inputs) == 1
        value = side_inputs[0]
        output = []
        for idx in inputs[0]:
            atr = object_with_unique_id(scene_structs[0], idx)[attribute]
            if value == atr or value in atr:
                output.append(idx)
        return output

    return filter_handler


def unique_handler(variations_outputs, scene_structs, causal_graph, inputs, side_inputs):
    assert len(inputs) == 1
    if len(inputs[0]) != 1:
        return '__INVALID__'
    return inputs[0][0]


def vg_relate_handler(variations_outputs, scene_structs, causal_graph, inputs, side_inputs):
    assert len(inputs) == 1
    assert len(side_inputs) == 1
    output = set()
    for rel in scene_structs[0]['relationships']:
        if rel['predicate'] == side_inputs[0] and rel['subject_idx'] == inputs[0]:
            output.add(rel['object_idx'])
    return sorted(list(output))


def relate_handler(variations_outputs, scene_structs, causal_graph, inputs, side_inputs):
    assert len(inputs) == 1
    assert len(side_inputs) == 1
    relation = side_inputs[0]
    return scene_structs[0]['relationships'][relation][inputs[0]]


def union_handler(variations_outputs, scene_structs, causal_graph, inputs, side_inputs):
    assert len(inputs) == 2
    assert len(side_inputs) == 0
    return sorted(list(set(inputs[0]) | set(inputs[1])))


def intersect_handler(variations_outputs, scene_structs, causal_graph, inputs, side_inputs):
    assert len(inputs) == 2
    assert len(side_inputs) == 0
    return sorted(list(set(inputs[0]) & set(inputs[1])))

def intersect_list_handler(variations_outputs, scene_structs, causal_graph, inputs, side_inputs):
    assert len(inputs) == 2
    assert len(side_inputs) == 0
    ret = []
    for objectList in inputs[0]:
        ret.append(sorted(list(set(objectList) & set(inputs[1]))))
    return ret

def difference_handler(variations_outputs, scene_structs, causal_graph, inputs, side_inputs):
    assert len(inputs) == 2
    assert len(side_inputs) == 0
    return sorted(list(set(inputs[0]) - set(inputs[1])))


def count_handler(variations_outputs, scene_structs, causal_graph, inputs, side_inputs):
    assert len(inputs) == 1
    return len(inputs[0])


def make_same_attr_handler(attribute):
    def same_attr_handler(variations_outputs, scene_structs, causal_graph, inputs, side_inputs):
        cache_key = '_same_%s' % attribute
        if cache_key not in scene_structs[0]:
            cache = {}
            for i, obj1 in enumerate(scene_structs[0]['objects']):
                same = []
                for j, obj2 in enumerate(scene_structs[0]['objects']):
                    if i != j and obj1[attribute] == obj2[attribute]:
                        same.append(j)
                cache[i] = same
            scene_structs[0][cache_key] = cache

        cache = scene_structs[0][cache_key]
        assert len(inputs) == 1
        assert len(side_inputs) == 0
        return cache[inputs[0]]

    return same_attr_handler


def make_query_handler(attribute):
    def query_handler(variations_outputs, scene_structs, causal_graph, inputs, side_inputs):
        assert len(inputs) == 1
        assert len(side_inputs) == 0
        idx = inputs[0]
        obj = object_with_unique_id(scene_structs[0], idx)
        assert attribute in obj
        val = obj[attribute]
        if type(val) == list and len(val) != 1:
            return '__INVALID__'
        elif type(val) == list and len(val) == 1:
            return val[0]
        else:
            return val

    return query_handler

def exist_handler(variations_outputs, scene_structs, causal_graph, inputs, side_inputs):
    assert len(inputs) == 1
    assert len(side_inputs) == 0
    return len(inputs[0]) > 0

def exist_list_handler(variations_outputs, scene_structs, causal_graph, inputs, side_inputs):
    assert len(inputs) == 1
    assert len(side_inputs) == 0
    ret = []
    for li in inputs[0]:
        ret.append(len(li) > 0)
    return ret

def any_false_handler(variations_outputs, scene_structs, causal_graph, inputs, side_inputs):
    assert len(inputs) == 1
    assert len(side_inputs) == 0
    ret = False in inputs[0]
    return ret

def any_true_handler(variations_outputs, scene_structs, causal_graph, inputs, side_inputs):
    assert len(inputs) == 1
    assert len(side_inputs) == 0
    ret = True in inputs[0]
    return ret

def equal_handler(variations_outputs, scene_structs, causal_graph, inputs, side_inputs):
    assert len(inputs) == 2
    assert len(side_inputs) == 0
    return inputs[0] == inputs[1]


def less_than_handler(variations_outputs, scene_structs, causal_graph, inputs, side_inputs):
    assert len(inputs) == 2
    assert len(side_inputs) == 0
    return inputs[0] < inputs[1]


def greater_than_handler(variations_outputs, scene_structs, causal_graph, inputs, side_inputs):
    assert len(inputs) == 2
    assert len(side_inputs) == 0
    return inputs[0] > inputs[1]

def events_handler(variations_outputs, scene_structs, causal_graph, inputs, side_inputs):
    return causal_graph.events

def filter_events_handler(variations_outputs, scene_structs, causal_graph, inputs, side_inputs):
    assert len(inputs) == 2
    return [event for event in inputs[0] if inputs[1] in event['objects']]

def make_filter_events_handler(event_type):
    def event_type_filter_handler(variations_outputs, scene_structs, causal_graph, inputs, side_inputs):
        assert len(inputs) == 1
        assert len(side_inputs) == 0
        return [event for event in inputs[0] if event['type'] == event_type]

    return event_type_filter_handler

def make_filter_events_list_handler(event_type):
    def event_type_filter_handler(variations_outputs, scene_structs, causal_graph, inputs, side_inputs):
        assert len(inputs) == 1
        assert len(side_inputs) == 0
        ret = []
        for variation_simulation in inputs[0]:
            ret.append([event for event in variation_simulation if event['type'] == event_type])
        return ret

    return event_type_filter_handler

def filter_collide_ground_handler(variations_outputs, scene_structs, causal_graph, inputs, side_inputs):
    assert len(inputs) == 1
    assert len(side_inputs) == 0
    collision_events = [event for event in inputs[0] if event['type'] == 'Collision']
    return [event for event in collision_events if is_ground(scene_structs, event['objects'][0]) or is_ground(scene_structs, event['objects'][1])]

def filter_collide_ground_list_handler(variations_outputs, scene_structs, causal_graph, inputs, side_inputs):
    assert len(inputs) == 1
    assert len(side_inputs) == 0
    ret = []
    for variation_simulation in inputs[0]:
        collision_events = [event for event in variation_simulation if event['type'] == 'Collision']
        ret.append([event for event in collision_events if is_ground(scene_structs, event['objects'][0]) or is_ground(scene_structs, event['objects'][1])])
    return ret

def filter_collide_basket_handler(variations_outputs, scene_structs, causal_graph, inputs, side_inputs):
    assert len(inputs) == 1
    assert len(side_inputs) == 0
    collision_events = [event for event in inputs[0] if event['type'] == 'Collision']
    return [event for event in collision_events if is_basket(scene_structs, event['objects'][0]) or is_basket(scene_structs, event['objects'][1])]

def filter_collide_basket_list_handler(variations_outputs, scene_structs, causal_graph, inputs, side_inputs):
    assert len(inputs) == 1
    assert len(side_inputs) == 0
    ret = []
    for variation_simulation in inputs[0]:
        collision_events = [event for event in variation_simulation if event['type'] == 'Collision']
        ret.append([event for event in collision_events if is_basket(scene_structs, event['objects'][0]) or is_basket(scene_structs, event['objects'][1])])
    return ret

def make_filter_events_with_dynamics_handler(event_type):
    def event_type_filter_handler(variations_outputs, scene_structs, causal_graph, inputs, side_inputs):
        assert len(inputs) == 1
        assert len(side_inputs) == 0
        return [event for event in inputs[0] if event['type'] == event_type and is_object_dynamic(object_with_unique_id(scene_structs[0], event['objects'][0])) and (len(event['objects']) == 1 or is_object_dynamic(object_with_unique_id(scene_structs[0], event['objects'][1])))]

    return event_type_filter_handler

def filter_first_handler(variations_outputs, scene_structs, causal_graph, inputs, side_inputs):
    assert len(inputs) == 1
    assert len(side_inputs) == 0
    assert len(inputs[0]) > 0
    return inputs[0][0]

def filter_last_handler(variations_outputs, scene_structs, causal_graph, inputs, side_inputs):
    assert len(inputs) == 1
    assert len(side_inputs) == 0
    assert len(inputs[0]) > 0
    return inputs[0][len(inputs[0])-1]

def event_partner_handler(variations_outputs, scene_structs, causal_graph, inputs, side_inputs):
    assert len(inputs) == 2
    assert len(inputs[1]['objects']) == 2
    assert len(side_inputs) == 0
    if inputs[1]['objects'][0] != inputs[0]:
        return inputs[1]['objects'][0]
    return inputs[1]['objects'][1]

def filter_moving_objects_handler(variations_outputs, scene_structs, causal_graph, inputs, side_inputs):
    assert len(inputs) == 2
    scene_struct = scene_structs[inputs[1]]
    return [objIdx for objIdx in inputs[0] if is_object_moving(object_with_unique_id(scene_struct, objIdx))]

def filter_stationary_objects_handler(variations_outputs, scene_structs, causal_graph, inputs, side_inputs):
    assert len(inputs) == 2
    scene_struct = scene_structs[inputs[1]]
    return [objIdx for objIdx in inputs[0] if not is_object_moving(object_with_unique_id(scene_struct, objIdx))]

def filter_dynamic_objects_handler(variations_outputs, scene_structs, causal_graph, inputs, side_inputs):
    assert len(inputs) == 1
    return [objIdx for objIdx in inputs[0] if is_object_dynamic(object_with_unique_id(scene_structs[0], objIdx))]

def start_scene_step_handler(variations_outputs, scene_structs, causal_graph, inputs, side_inputs):
    assert len(inputs) == 0
    return 0

def end_scene_step_handler(variations_outputs, scene_structs, causal_graph, inputs, side_inputs):
    assert len(inputs) == 0
    return -1

def filter_objects_from_events_handler(variations_outputs, scene_structs, causal_graph, inputs, side_inputs):
    assert len(inputs) == 1
    ret = set()
    for event in inputs[0]:
        ret.add(event['objects'][0])
        ret.add(event['objects'][1])
    return list(ret)

def filter_objects_from_events_list_handler(variations_outputs, scene_structs, causal_graph, inputs, side_inputs):
    assert len(inputs) == 1
    ret = []
    for variation_simulation in inputs[0]:
        sim_ret = set()
        for event in variation_simulation:
            sim_ret.add(event['objects'][0])
            sim_ret.add(event['objects'][1])
        ret.append(list(sim_ret))
    return ret

def counterfact_events_handler(variations_outputs, scene_structs, causal_graph, inputs, side_inputs):
    assert len(inputs) == 1
    object_removed_variation_simulation = variations_outputs['variations_outputs'][str(inputs[0])]
    variation_causal_graph = CausalGraph(object_removed_variation_simulation['causal_graph'])
    return variation_causal_graph.events

def counterfact_events_list_handler(variations_outputs, scene_structs, causal_graph, inputs, side_inputs):
    assert len(inputs) == 1
    ret = []
    for uniqueObjectIdx in inputs[0]:
        object_removed_variation_simulation = variations_outputs['variations_outputs'][str(uniqueObjectIdx)]
        ret.append(CausalGraph(object_removed_variation_simulation['causal_graph']).events)
    return ret

def filter_before_handler(variations_outputs, scene_structs, causal_graph, inputs, side_inputs):
    assert len(inputs) == 2
    return [event for event in inputs[0] if event['step'] < inputs[1]['step']]

def filter_after_handler(variations_outputs, scene_structs, causal_graph, inputs, side_inputs):
    assert len(inputs) == 2
    return [event for event in inputs[0] if event['step'] > inputs[1]['step']]

def is_before_handler(variations_outputs, scene_structs, causal_graph, inputs, side_inputs):
    assert len(inputs) == 2
    return inputs[0]['step'] < inputs[1]['step']

def is_after_handler(variations_outputs, scene_structs, causal_graph, inputs, side_inputs):
    assert len(inputs) == 2
    return inputs[0]['step'] > inputs[1]['step']

def as_list_handler(variations_outputs, scene_structs, causal_graph, inputs, side_inputs):
    assert len(inputs) == 1
    return [inputs[0]]


# Register all of the answering handlers here.
# TODO maybe this would be cleaner with a function decorator that takes
# care of registration? Not sure. Also what if we want to reuse the same engine
# for different sets of node types?
execute_handlers = {
    'start_scene': start_scene_handler,
    'end_scene': end_scene_handler,
    'filter_color': make_filter_handler('color'),
    'filter_shape': make_filter_handler('shape'),
    'filter_size': make_filter_handler('size'),
    'filter_objectcategory': make_filter_handler('objectcategory'),
    'unique': unique_handler,
    'relate': relate_handler,
    'union': union_handler,
    'intersect': intersect_handler,
    'intersect_list': intersect_list_handler,
    'difference': difference_handler,
    'count': count_handler,
    'query_color': make_query_handler('color'),
    'query_shape': make_query_handler('shape'),
    'query_size': make_query_handler('size'),
    'exist': exist_handler,
    'event_exist': exist_handler,
    'exist_list': exist_list_handler,
    'any_false': any_false_handler,
    'any_true': any_true_handler,
    'equal_color': equal_handler,
    'equal_shape': equal_handler,
    'equal_integer': equal_handler,
    'equal_size': equal_handler,
    'equal_object': equal_handler,
    'less_than': less_than_handler,
    'greater_than': greater_than_handler,
    'same_color': make_same_attr_handler('color'),
    'same_shape': make_same_attr_handler('shape'),
    'same_size': make_same_attr_handler('size'),
    'events': events_handler,
    'filter_events': filter_events_handler,
    'filter_collision': make_filter_events_handler('Collision'),
    'filter_collision_with_dynamics': make_filter_events_with_dynamics_handler('Collision'),
    'filter_enter_container': make_filter_events_handler('ContainerEndUp'),
    'filter_enter_container_list': make_filter_events_list_handler('ContainerEndUp'),
    'filter_collide_ground': filter_collide_ground_handler,
    'filter_collide_ground_list': filter_collide_ground_list_handler,
    'filter_collide_basket': filter_collide_basket_handler,
    'filter_collide_basket_list': filter_collide_basket_list_handler,
    'filter_first': filter_first_handler,
    'filter_last': filter_last_handler,
    'event_partner': event_partner_handler,
    'filter_moving_objects': filter_moving_objects_handler,
    'filter_stationary_objects': filter_stationary_objects_handler,
    'filter_objects_from_events': filter_objects_from_events_handler,
    'filter_objects_from_events_list': filter_objects_from_events_list_handler,
    'filter_dynamic_objects': filter_dynamic_objects_handler,
    'start_scene_step': start_scene_step_handler,
    'end_scene_step': end_scene_step_handler,
    'counterfact_events': counterfact_events_handler,
    'counterfact_events_list': counterfact_events_list_handler,
    'filter_before': filter_before_handler,
    'filter_after': filter_after_handler,
    'is_before': is_before_handler,
    'is_after': is_after_handler,
    'as_list': as_list_handler
}


def answer_question(question, metadata, variations_outputs, scene_structs, causal_graph, all_outputs=False,
                    cache_outputs=True):
    """
    Use structured scene information to answer a structured question. Most of the
    heavy lifting is done by the execute handlers defined above.

    We cache node outputs in the node itself; this gives a nontrivial speedup
    when we want to answer many questions that share nodes on the same scene
    (such as during question-generation DFS). This will NOT work if the same
    nodes are executed on different scenes.
    """
    all_input_types, all_output_types = [], []
    node_outputs = []
    for node in question['nodes']:
        if cache_outputs and '_output' in node:
            node_output = node['_output']
        else:
            node_type = node['type']
            msg = 'Could not find handler for "%s"' % node_type
            assert node_type in execute_handlers, msg
            handler = execute_handlers[node_type]
            node_inputs = [node_outputs[idx] for idx in node['inputs']]
            side_inputs = node.get('side_inputs', [])
            node_output = handler(variations_outputs, scene_structs, causal_graph, node_inputs, side_inputs)
            if cache_outputs:
                node['_output'] = node_output
        node_outputs.append(node_output)
        if node_output == '__INVALID__':
            break

    if all_outputs:
        return node_outputs
    else:
        return node_outputs[-1]


def insert_scene_node(nodes, idx):
    # First make a shallow-ish copy of the input
    new_nodes = []
    for node in nodes:
        new_node = {
            'type': node['type'],
            'inputs': node['inputs'],
        }
        if 'side_inputs' in node:
            new_node['side_inputs'] = node['side_inputs']
        new_nodes.append(new_node)

    # Replace the specified index with a scene node
    new_nodes[idx] = {'type': 'scene', 'inputs': []}

    # Search backwards from the last node to see which nodes are actually used
    output_used = [False] * len(new_nodes)
    idxs_to_check = [len(new_nodes) - 1]
    while idxs_to_check:
        cur_idx = idxs_to_check.pop()
        output_used[cur_idx] = True
        idxs_to_check.extend(new_nodes[cur_idx]['inputs'])

    # Iterate through nodes, keeping only those whose output is used;
    # at the same time build up a mapping from old idxs to new idxs
    old_idx_to_new_idx = {}
    new_nodes_trimmed = []
    for old_idx, node in enumerate(new_nodes):
        if output_used[old_idx]:
            new_idx = len(new_nodes_trimmed)
            new_nodes_trimmed.append(node)
            old_idx_to_new_idx[old_idx] = new_idx

    # Finally go through the list of trimmed nodes and change the inputs
    for node in new_nodes_trimmed:
        new_inputs = []
        for old_idx in node['inputs']:
            new_inputs.append(old_idx_to_new_idx[old_idx])
        node['inputs'] = new_inputs

    return new_nodes_trimmed


def is_degenerate(question, metadata, scene_structs, causal_graph, answer=None, verbose=False):
    """
    A question is degenerate if replacing any of its relate nodes with a scene
    node results in a question with the same answer.
    """
    if answer is None:
        answer = answer_question(question, metadata, scene_structs, causal_graph)

    for idx, node in enumerate(question['nodes']):
        if node['type'] == 'relate':
            new_question = {
                'nodes': insert_scene_node(question['nodes'], idx)
            }
            new_answer = answer_question(new_question, metadata, scene_structs, causal_graph)
            if verbose:
                print('here is truncated question:')
                for i, n in enumerate(new_question['nodes']):
                    name = n['type']
                    if 'side_inputs' in n:
                        name = '%s[%s]' % (name, n['side_inputs'][0])
                    print(i, name, n['_output'])
                print('new answer is: ', new_answer)

            if new_answer == answer:
                return True

    return False
