<?php

/**
 * @file
 * Page callback file for the download_count module export feature.
 */

/**
 * Download_count export form.
 */
function download_count_export_form($form, $form_state, $dc_entry = NULL) {
  drupal_add_js(drupal_get_path('module', 'download_count') . '/download_count.js');
  if ($dc_entry == 'all') {
    drupal_set_title(t('Download Count Export CSV - All Files'));
  }
  else {
    drupal_set_title(t('Download Count Export CSV - %filename on %type %id', array('%filename' => $dc_entry->filename, '%type' => $dc_entry->type, '%id' => $dc_entry->id)));
  }

  $form['download_count_export_note'] = array(
    '#type' => 'markup',
    '#value' => t('The following data will be exported:')
      . '<ul>'
      . '<li>' . t('Download count id')
      . '<li>' . t('File id')
      . '<li>' . t('File name')
      . '<li>' . t('File size')
      . '<li>' . t('Entity type')
      . '<li>' . t('Entity id ')
      . '<li>' . t('Downloading user id')
      . '<li>' . t('Downloading username')
      . '<li>' . t('Downloading user ip address')
      . '<li>' . t('HTTP referrer')
      . '<li>' . t('Date - time (YYYY-MM-DD  HH:MM:SS)')
      . '</ul>',
  );
  $form['download_count_export_range'] = array(
    '#type' => 'radios',
    '#title' => t('Export Range'),
    '#options' => array(
      t('export all data'),
      t('export data for a specified date range'),
    ),
    '#default_value' => variable_get('download_count_export_range', 0),
  );
  $form['download_count_export_date_range_from'] = array(
    '#type' => 'date',
    '#title' => t('Export Range From Date'),
    '#description' => t('This field will be ignored if the Export Range \'export all data\' option is selected above.'),
  );
  $form['download_count_export_date_range_to'] = array(
    '#type' => 'date',
    '#title' => t('Export Range To Date'),
    '#description' => t('This field will be ignored if the Export Range \'export all data\' option is selected above.'),
  );
  $form['download_count_file_info'] = array(
    '#type' => 'value',
    '#value' => $dc_entry,
  );
  $form['download_count_export_submit'] = array(
    '#type' => 'submit',
    '#value' => t('Export'),
  );
  $form['download_count_export_cancel'] = array(
    '#value' => '<a href="javascript:history.back(-1)">' . t('Cancel') . '</a>',
  );

  $form_state['#redirect'] = TRUE;
  return $form;
}

/**
 * Implements hook_submit().
 */
function download_count_export_form_submit($form, &$form_state) {
  variable_set('download_count_export_range', $form_state['values']['download_count_export_range']);
  $export_file_path = 'sites/default/files/download_count_export_' . ($form_state['values']['download_count_file_info'] == 'all' ? 'all_files' : check_plain($form_state['values']['download_count_file_info']->filename)) . '_' . date('Y-m-d') . '.csv';
  download_count_export_file($export_file_path, $form_state['values']['download_count_export_range'], $form_state['values']['download_count_file_info'], $form_state['values']['download_count_export_date_range_from'], $form_state['values']['download_count_export_date_range_to']);

  if (!file_exists($export_file_path)) {
    drupal_set_message("$export_file_path does not exist.", 'error');
    return;
  }
  if (!($fp = fopen($export_file_path, "r"))) {
    drupal_set_message("Could not open $export_file_path for reading.", 'error');
    return;
  }
  if (($file_size = filesize($export_file_path)) == 0) {
    drupal_set_message("$export_file_path is empty.", 'error');
    return;
  }

  //drupal_add_http_header('Content-Disposition', 'attachment; filename="' . basename($export_file_path) . '"');
  //drupal_add_http_header('Content-Length', $file_size);
  //drupal_add_http_header('Content-Type', 'application/csv; charset=utf-8');
  //drupal_add_http_header('CacheControl', 'no-cache');
  //drupal_add_http_header('Pragma', 'no-cache');
  //drupal_add_http_header('Expires', '-1');

 $headers = array(
 array('key' => 'Content-Type', 'value' => 'application/octet-stream'),
 array('key' => 'Content-Disposition', 'value' => 'attachment; filename="' . basename($export_file_path) . '"'),
 array('key' => 'Content-Length', 'value' => $file_size),
 );
 foreach ($headers as $header) {
 // prevent HTTP header injection
 $header['value'] = preg_replace('/\r?\n(?!\t| )/', '', $header['value']);
 drupal_add_http_header($header['key'], $header['value']);
 }

  ob_clean();
  flush();
  readfile($export_file_path);
  unlink($export_file_path);

  drupal_set_message(basename($export_file_path) . " has been successfully exported.", 'status');
  return;
}

/**
 * Creates the temporary file for export.
 *
 * @param $file_path
 *   TODO.
 * @param $range
 *   TODO.
 * @param $file_info
 *   TODO.
 * @param $start
 *   TODO.
 * @param $end
 *   TODO.
 *
 * @return
 *   TRUE if the file was created, and FALSE if it wasn't.
 */
function download_count_export_file($file_path, $range, $file_info, $start, $end) {

  if (!($fp = fopen($file_path, "w"))) {
    drupal_set_message("Could not open $file_path for writing.", 'error');
    return FALSE;
  }

  if ($range == 0) {
    if ($file_info == 'all') {
      $result = db_query('SELECT dc.dcid, dc.fid, dc.type, dc.id, dc.uid, dc.ip_address, dc.referrer, dc.timestamp, f.filename, f.filesize, f.uri, u.name FROM {download_count} dc JOIN {file_managed} f ON dc.fid = f.fid JOIN {users} u ON dc.uid = u.uid ORDER BY f.filename');
    }
    else {
      $result = db_query('SELECT dc.dcid, dc.fid, dc.type, dc.id, dc.uid, dc.ip_address, dc.referrer, dc.timestamp, f.filename, f.filesize, f.uri, u.name FROM {download_count} dc JOIN {file_managed} f ON dc.fid = f.fid JOIN {users} u ON dc.uid = u.uid WHERE dc.fid = :dc.fid', array(':dc.fid' => $file_info->fid));
    }
  }
  else {
    $from = strtotime($start['year'] . '-' . $start['month'] . '-' . $start['day']);
    $to = strtotime($end['year'] . '-' . $end['month'] . '-' . $end['day']);
    if ($from == $to) {
      $to += 86400;
    }
    if ($file_info == 'all') {
      $result = db_query('SELECT dc.dcid, dc.fid, dc.type, dc.id, dc.uid, dc.ip_address, dc.referrer, dc.timestamp, f.filename, f.filesize, f.uri, u.name FROM {download_count} dc JOIN {file_managed} f ON dc.fid = f.fid JOIN {users} u ON dc.uid = u.uid WHERE dc.timestamp >= :dc.timestamp AND dc.timestamp <= :dc.timestamp ORDER BY f.filename', array(':dc.timestamp' => $from, ':dc.timestamp' => $to));
    }
    else {
      $result = db_query('SELECT dc.dcid, dc.fid, dc.type, dc.id, dc.uid, dc.ip_address, dc.referrer, dc.timestamp, f.filename, f.filesize, f.uri, u.name FROM {download_count} dc JOIN {file_manage} f ON dc.fid = f.fid JOIN {users} u ON dc.uid = u.uid WHERE dc.fid = :dc.fid AND (dc.timestamp >= :dc.timestamp_from AND dc.timestamp <= :dc.timestamp_to)', array(':dc.fid' => $file_info->fid, ':dc.timestamp_from' => $from, ':dc.timestamp_to' => $to));
    }
  }

  $column_names = '"Download count id","File id","File name","File URI", "File size","Entity type", "Entity id","Downloading user id","Downloading username","Downloading user ip address","HTTP referrer","Date time"' . "\n";
  fwrite($fp, $column_names);
  foreach ($result as $record) {
    $row  = '"' . $record->dcid . '"' . ',';
    $row .= '"' . $record->fid . '"' . ',';
    $row .= '"' . $record->filename . '"' . ',';
    $row .= '"' . $record->uri . '"' . ',';
    $row .= '"' . $record->filesize . '"' . ',';
    $row .= '"' . $record->type . '"' . ',';
    $row .= '"' . $record->id . '"' . ',';
    $row .= '"' . $record->uid . '"' . ',';
    $row .= '"' . $record->name . '"' . ',';
    $row .= '"' . $record->ip_address . '"' . ',';
    $row .= '"' . $record->referrer . '"' . ',';
    $row .= '"' . date('Y-m-d H:i:s', $record->timestamp) . '"';
    $row .= "\n";
    fwrite($fp, $row);
  }
  fclose($fp);
  return TRUE;
}
