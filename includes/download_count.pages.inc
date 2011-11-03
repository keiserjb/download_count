<?php

/**
 * @file
 * Administrative page callbacks for the download_count module.
 */

/**
 * Page callback for the view download counts page.
 */
function download_count_view_page($arg = NULL) {

  drupal_set_title(variable_get('download_count_view_page_title', 'Download Counts'));
  global $user;
  $rows = array();
  $header = array();
  $result = NULL;
  $total_downloads = 0;
  $files = array();
  $colspan = 0;
  $limit = (int) variable_get('download_count_view_page_limit', 0);
  $items_per_page = (int) variable_get('download_count_view_page_items', 0);
  $page_header = variable_get('download_count_view_page_header', '');
  $page_footer = variable_get('download_count_view_page_footer', '');
  $output = '<div id="download-count-page">';

  $header[] = array(
    'data' => t('Count'),
    'field' => t('count'),
    'sort' => t('desc'),
  );
  $header[] = array(
    'data' => t('Entity Type'),
    'field' => t('type'),
  );
  $header[] = array(
    'data' => t('Bundle Type'),
    'field' => t('bundle'),
  );
  $header[] = array(
    'data' => t('Entity ID'),
    'field' => t('id'),
  );
  $header[] = array(
    'data' => t('File name'),
    'field' => t('filename'),
  );
  $header[] = array(
    'data' => t('Size'),
    'field' => t('filesize'),
  );
  $header[] = array(
    'data' => t('Last Downloaded'),
    'field' => t('last'),
  );

  $query = db_select('download_count', 'dc')
    ->fields('dc', array('dcid', 'type', 'bundle','id'))
    ->fields('f', array('filename', 'fid', 'filesize', 'uri'))
    ->groupBy('dc.type')
    ->groupBy('dc.id')
    ->groupBy('f.filename');
  if ($limit > 0) {
    $query->range(0, $limit);
  }
  $query->addExpression('COUNT(dc.dcid)', 'count');
  $query->addExpression('MAX(dc.timestamp)', 'last');
  $query->join('file_managed', 'f', 'dc.fid = f.fid');
  if ($items_per_page > 0) {
    $query = $query->extend('PagerDefault')->limit($items_per_page);
  }
  $query = $query->extend('TableSort')->orderByHeader($header);

  $result = $query->execute();
  foreach ($result as $file) {
    $row = array();
    $row[] = $file->count;
    $row[] = $file->type;
    $row[] = $file->bundle;
    $row[] = $file->id;
    $row[] = $file->filename;
    $row[] = format_size($file->filesize);
    $row[] = t('@time ago', array('@time' => format_interval(REQUEST_TIME - $file->last)));
    $row[] = l(t('Details'), 'admin/reports/download-count/' . $file->dcid . '/details');
    user_access('export download counts') ? $row[] = l(t('Export'), 'admin/reports/download-count/' . $file->dcid . '/export') : NULL;
    user_access('administer site configuration') ? $row[] = l(t('Reset'), 'admin/reports/download-count/' . $file->dcid . '/reset') : NULL;
    $rows[] = $row;

    $total_downloads += $file->count;
    $files[] = $file->filename;
    $colspan = count($rows[0]) - count($header);
  }

  if (empty($rows)) {
    $rows[] = array(array(
        'data' => t('No files have been downloaded.'),
        'colspan' => '4',
      ));
  }

  if ($colspan > 1) {
    $header[] = array(
      'data' => t('Action(s)')
        . (user_access('export download counts') || user_access('administer site configuration') ? '&nbsp;&nbsp;[' : NULL)
        . (user_access('export download counts') ? l(t('Export All'), 'admin/reports/download-count/all/export') : NULL)
        . (user_access('export download counts') && user_access('administer site configuration') ? ' / ' : NULL)
        . (user_access('administer site configuration') ? l(t('Reset All'), 'admin/reports/download-count/all/reset') : NULL)
        . (user_access('export download counts') || user_access('administer site configuration') ? ']' : NULL),
      'colspan' => $colspan,
    );
    drupal_add_css(drupal_get_path('module', 'download_count') . '/download_count.css');
  }

  $output .= check_markup($page_header['value'], $page_header['format']);
  $output .= '<div id="download-count-summary-top">' . t('Unique Files:') . ' ' . count(array_unique($files)) . ', ' . t('Total Downloads:') . ' ' . $total_downloads . '</div>';
  $output .= theme('table', array('header' => $header, 'rows' => $rows, 'attributes' => array('id' => 'download-count-table')));
  $output .= '<div id="download-count-summary-bottom">' . t('Unique Files:') . ' ' . count(array_unique($files)) . ', ' . t('Total Downloads:') . ' ' . $total_downloads . '</div>';
  if ($items_per_page > 0) {
    $output .= theme('pager',array('tags' => array()));
  }
  $output .= check_markup($page_footer['value'], $page_footer['format']);

  $output .= '</div>';
  return $output;
}

/**
 * Download count reset function.
 */
function download_count_reset_form($form, $form_state, $dc_entry = NULL) {
  $form = array();
  if ($dc_entry != 'all') {
    $form['dcid'] = array(
      '#type' => 'value',
      '#value' => $dc_entry->dcid,
    );
    $form['filename'] = array(
      '#type' => 'value',
      '#value' => $dc_entry->filename,
    );
    $form['fid'] = array(
      '#type' => 'value',
      '#value' => $dc_entry->fid,
    );
    $form['type'] = array(
      '#type' => 'value',
      '#value' => $dc_entry->type,
    );
    $form['bundle'] = array(
      '#type' => 'value',
      '#value' => $dc_entry->bundle,
    );
    $form['id'] = array(
      '#type' => 'value',
      '#value' => $dc_entry->id,
    );
    return confirm_form(
      $form,
      t('Are you sure you want to reset the download count for %filename?', array('%filename' => $dc_entry->filename)),
      'admin/reports/download-count',
      t('This action cannot be undone.'),
      t('Reset'),
      t('Cancel')
    );
  }
  else {
    $form['dcid'] = array(
      '#type' => 'value',
      '#value' => 'all',
    );
    return confirm_form(
      $form,
      t('Are you sure you want to reset all download counts?'),
      'admin/reports/download-count',
      t('This action cannot be undone.'),
      t('Reset All'),
      t('Cancel')
    );
  }
}

/**
 * Implements hook_submit().
 */
function download_count_reset_form_submit($form, &$form_state) {
  $result = NULL;
  if ($form['dcid']['#value'] == 'all') {

    $result = db_delete('download_count')->execute();
    if ($result) {
      drupal_set_message(t('All download counts have been reset.'));
      watchdog('download_count', 'All download counts have been reset.', WATCHDOG_NOTICE);
    }
    else {
      drupal_set_message(t('Unable to reset all download counts.'), 'error');
      watchdog('download_count', 'Unable to reset all download counts.', WATCHDOG_ERROR);
    }
  }
  else {
    $result = db_delete('download_count')
      ->condition('fid', $form['fid']['#value'])
      ->condition('type', $form['type']['#value'])
      ->condition('id', $form['id']['#value'])
      ->execute();
    if ($result) {
      drupal_set_message(t('Download count for %filename on %type %id was reset.', array('%filename' => $form['filename']['#value'], '%type' => $form['type']['#value'], '%id' => $form['id']['#value'])));
      watchdog('download_count', 'Download count for %filename on %type %id was reset.', array('%filename' => $form['filename']['#value'], '%type' => $form['type']['#value'], '%id' => $form['id']['#value']), WATCHDOG_NOTICE);
    }
    else {
      drupal_set_message(t('Unable to reset download count for %filename on %type %id.', array('%filename' => $form['filename']['#value'], '%type' => $form['type']['#value'], '%id' => $form['id']['#value'])), 'error');
      watchdog('download_count', 'Unable to reset download count for %filename on %type %id.', array('%filename' => $form['filename']['#value'], '%type' => $form['type']['#value'], '%id' => $form['id']['#value']), WATCHDOG_ERROR);
    }
  }
  $form_state['redirect'] = 'admin/reports/download-count';
}

function download_count_view_details($arg = NULL) {

  drupal_set_title('Download Count Details for ' . $arg->filename . ' on ' . $arg->type . ' ' . $arg->id);
  $output = l(t('&#0171; Back to summary'), 'admin/reports/download-count', array ('html' => true));
  $output .= '<br />TBD - WORK IN PROGRESS';

  return $output;
}
