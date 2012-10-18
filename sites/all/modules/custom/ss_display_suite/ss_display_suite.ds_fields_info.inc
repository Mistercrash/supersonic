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
  $form = drupal_get_form('ss_display_suite_point_generation_form');

  $entity = $field['entity'];

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
  $nid = arg(1);
  $entity = entity_load('node', nid);
  $points = field_get_items('node', $entity, 'field_total_points', $langcode = NULL);
  dpm($points);
}
