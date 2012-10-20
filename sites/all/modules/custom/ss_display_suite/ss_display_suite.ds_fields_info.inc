<?php
/**
 * @file
 * TODO: Enter file description here.
 */

/**
 * Implements hook_ds_fields_info().
 */
function ss_display_suite_ds_fields_info($entity_type) {
  $fields = array();

  $fields['point_generation_form'] = array(
    'title' => t('Point generation form field'),
    'field_type' => DS_FIELD_TYPE_FUNCTION,
    'ui_limit' => array('jump|*'),
    'function' => 'ss_display_suite_ds_field_point_generation_form',
  );

  return array('node' => $fields);
}

/**
* ds field: point generation form
* Returns a form that can generate points for a jump if none are created yet
*/
function ss_display_suite_ds_field_point_generation_form($field) {
  // Pass the node to the form
  $entity = $field['entity'];
  $form_state['#node'] = $entity;
  $form = drupal_build_form('ss_display_suite_point_generation_form', $form_state);

  return drupal_render($form);
}

function ss_display_suite_point_generation_form($form, &$form_state) {
  $form['start_time'] = array(
      '#type' => 'select',
      '#title' => t('Start time'),
      '#multiple' => TRUE,
      '#options' => array(),
      '#required' => TRUE,
      '#validated' => TRUE,
  );
  $form['add_point'] = array(
    '#type' => 'button',
    '#value' => t('Start'),
  );
  $form['points_total'] = array(
    '#type' => 'textfield',
    '#title' => t('Total points'),
    '#size' => 2,
    '#default_value' => 0,
    '#maxlength' => 2,
    '#disabled' => TRUE,
  );
  $form['clear_form'] = array(
    '#type' => 'button',
    '#value' => t('Clear form'),
  );
  $form['point_markers'] = array(
    '#type' => 'select',
    '#multiple' => TRUE,
    '#options' => array(),
    '#title' => t('Point markers'),
    '#required' => TRUE,
    '#validated' => TRUE,
  );
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Generate points'),
  );
  return $form;
}
function ss_display_suite_point_generation_form_validate($form, &$form_state) {
//   if(empty($form_state['values']['start_time'])) {
//     form_set_error('start_time', 'You must enter a start time.');
//   }
//   if(empty($form_state['values']['point_markers'])) {
//     form_set_error('point_markers', 'You must set at least one point for a jump.');
//   }
}
function ss_display_suite_point_generation_form_submit($form, &$form_state) {
  $node = $form_state['#node'];
  $points = field_get_items('node', $node, 'field_points', $langcode = NULL);
  if (empty($points)) {
    if (isset($node->type) && $node->type == 'jump') {
      // Get language and file
      $language = $node->language;
      //$feeds_item = node_load($node->feeds_item->feed_nid);
      //$file = $node->field_filename[$language][0]['value'];
      // Split file into draw and total score
      //$info = pathinfo($file);
      //$filename = $info['filename'];
      //$clean_filename = trim(preg_replace('/\s*\([^)]*\)/', '', $filename));
      //$split = explode('=', $clean_filename);
      //$draw = explode('-', $split[0]);
      $draw = explode('-', $node->title);
      $points = array_values($form_state['values']['point_markers']);
      $total_points = count($points);

      $generated_points = ss_display_suite_generate_multiple_point_draw($draw, $total_points);
      $created_points = array();
      $i = 0;
      // Create a node for each point and reference it to the formation
      foreach ($generated_points as $key => $value) {
        // Set node values
        $point = new stdClass();
        $point->type = 'point';
        node_object_prepare($point);
        $point->title    = $value['title'];
        $point->language = LANGUAGE_NONE;
        $point->field_formation['und']['0']['target_id'] = $value['nid'];
        $point->field_timecode['und']['0']['value'] = $points[$i];
        //$point->group_audience['und']['0'] = $feeds_item->group_audience['und']['0'];
        // Create the node
        node_save($point);
        // Keep nid's to store in the node reference field
        $created_points[] = array('target_id' => $point->nid);
        $i++;
      }
      // Add the created points to the field
      $node->field_points['und'] = $created_points;
      // Fill in the total score
      $node->field_total_score['und']['0']['value'] = $total_points;
      // Get the formations from the draw and reference them for organising
      // purposes. This will be easy to look up jumps in history on draw.
      $draw_figures =  array();
      foreach ($draw as $figure) {
        $result = db_query('SELECT n.nid FROM {node} n WHERE n.title = :title and n.type = :type', array(':title' => $figure, ':type' => 'formation'));
        $figure_nid = $result->fetchField();
        $draw_figures[] = array('target_id' => (int)$figure_nid);
      }
      $node->field_formation['und'] = $draw_figures;
      // Copy some values from the feed
      //$node->field_dropzone = $feeds_item->field_dropzone;
      //$node->field_date = $feeds_item->field_date;
      //$node->field_training['und']['0']['target_id'] = $node->feeds_item->feed_nid;
      //$node->group_audience['und']['0'] = $feeds_item->group_audience['und']['0'];
      // Save the created node (necessary?)
      field_attach_update('node', $node);
    }
  }
  else {
  }
}

/**
*
* Helper function to generate all points that have been attempted in the jump
* @param array() $draw
* @param integer $points
* @return multitype:
*/
function ss_display_suite_generate_multiple_point_draw($draw, $points) {
  // Generate the single point draw
  $single_point_draw = array();
  foreach ($draw as $figure) {
    $result = db_query('SELECT n.nid FROM {node} n WHERE n.title = :title and n.type = :type', array(':title' => $figure, ':type' => 'formation'));
    $figure_nid = $result->fetchField();
    // Blocks get 2 points
    if(is_numeric($figure)) {
      $single_point_draw[] = array(
        'nid' => (int)$figure_nid,
        'title' => $figure,
      );
      $single_point_draw[] = array(
        'nid' => (int)$figure_nid,
        'title' => $figure,
      );
    }
    // Randoms only one
    else {
      $single_point_draw[] = array(
        'nid' => (int)$figure_nid,
        'title' => $figure,
      );
    }
  }
  // Get count of the single point draw
  $single_point_draw_count = count($single_point_draw);
  // If we have less points slice the array
  if ($points < $single_point_draw_count) {
    $multiple_point_draw = array_slice($single_point_draw, 0, $points);
  }
  // If its the same just copy it
  elseif ($points == $single_point_draw_count) {
    $multiple_point_draw = $single_point_draw;
  }
  // If its bigger multiply and add the sliced rest.
  else {
    // Generate as many figures as points
    $multiplier = ceil($points / $single_point_draw_count);
    $i = 0;
    $multiple_point_draw = array();
    while ($i < $multiplier) {
      $multiple_point_draw = array_merge($single_point_draw, $multiple_point_draw);
      $i++;
    }
    // Cut off excess points
    $multiple_point_draw = array_slice($multiple_point_draw, 0, $points);
  }

  // Return all the points we need to generate
  return array_values($multiple_point_draw);
}

