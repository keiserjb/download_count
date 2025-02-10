<?php

/**
 * @file
 * Administrative page callbacks for the download_count module.
 */

/**
 * Page callback for the view download counts page.
 */
function download_count_view_page($arg = NULL) {
  $config = config('download_count.settings');
  backdrop_set_title($config->get('download_count_view_page_title'));
  $total_downloads = 0;
  $colspan = 0;
  $item = 1;
  $limit = (int) $config->get('download_count_view_page_limit');
  $items_per_page = (int) $config->get('download_count_view_page_items');
  $page_header = $config->get('download_count_view_page_header');
  $page_footer = $config->get('download_count_view_page_footer');
  $output = '<div id="download-count-page">';

  $header = array(
    array(
      'data' => t('#'),
    ),
    array(
      'data' => t('Count'),
      'field' => 'count',
      'sort' => 'desc',
    ),
    array(
      'data' => t('FID'),
      'field' => 'FID',
    ),
    array(
      'data' => t('Entity Type'),
      'field' => 'type',
    ),
    array(
      'data' => t('Entity ID'),
      'field' => 'id',
    ),
    array(
      'data' => t('File name'),
      'field' => 'filename',
    ),
    array(
      'data' => t('File Size'),
      'field' => 'file-size',
    ),
    array(
      'data' => t('Total Size'),
      'field' => 'total-size',
    ),
    array(
      'data' => t('Last Downloaded'),
      'field' => 'last',
    ),
  );

  $query = db_select('download_count', 'dc')
    ->fields('dc', array('dcid', 'type', 'id'))
    ->fields('f', array('filename', 'fid', 'filesize', 'uri'))
    ->groupBy('dc.fid');
  $query->addExpression('COUNT(dc.dcid)', 'count');
  $query->addExpression('COUNT(dc.dcid) * f.filesize', 'total-size');
  $query->addExpression('MAX(dc.timestamp)', 'last');
  $query->join('file_managed', 'f', 'dc.fid = f.fid');

  if ($limit > 0) {
    $query->range(0, $limit);
  }

  if ($items_per_page > 0) {
    $query = $query->extend('PagerDefault')->limit($items_per_page);
  }
  $query = $query->extend('TableSort')->orderByHeader($header);

  $rows = array();
  $result = $query->execute();
  foreach ($result as $file) {
    $row = array();
    $row[] = $item;
    $row[] = number_format($file->count);
    $row[] = $file->fid;
    $row[] = check_plain($file->type);
    $row[] = $file->id;
    $row[] = check_plain($file->filename);
    $row[] = format_size($file->filesize);
    $row[] = format_size($file->count * $file->filesize);
    $row[] = t('@time ago', array('@time' => format_interval(REQUEST_TIME - $file->last)));
    $row[] = l(t('Details'), 'admin/reports/download-count/' . $file->dcid . '/details');
    if (user_access('export download counts')) {
      $row[] = l(t('Export'), 'admin/reports/download-count/' . $file->dcid . '/export');
    }
    if (user_access('reset download counts')) {
      $row[] = l(t('Reset'), 'admin/reports/download-count/' . $file->dcid . '/reset');
    }
    $rows[] = $row;
    $item++;
    $total_downloads += $file->count;
    $colspan = count($rows[0]) - count($header);
  }

  if (empty($rows)) {
    $rows[] = array(array(
      'data' => t('No files have been downloaded.'),
      'colspan' => '9',
    ));
  }

  if ($colspan > 1) {
    $header[] = array(
      'data' => t('Actions')
        . '&nbsp;&nbsp;['
        . l(t('Details All'), 'admin/reports/download-count/all/details')
        . (user_access('export download counts')?l(t('Export All'), 'admin/reports/download-count/all/export') : NULL)
        . (user_access('reset download counts')?l(t('Reset All'), 'admin/reports/download-count/all/reset') : NULL)
        . ']',
      'colspan' => $colspan,
      'id' => 'actions',
    );
    backdrop_add_css(backdrop_get_path('module', 'download_count') . '/download_count.css');
  }
  if (!empty($page_header['value'])) {
    $output .= '<div id="download-count-header">' . check_markup($page_header['value'], $page_header['format']) . '</div>';
  }
  $output .= '<div id="download-count-total-top">' . t('Total Downloads:') . ' ' . number_format($total_downloads) . '</div>';
  $output .= theme('table', array('header' => $header, 'rows' => $rows, 'attributes' => array('id' => 'download-count-table')));
  $output .= '<div id="download-count-total-bottom">' . t('Total Downloads:') . ' ' . number_format($total_downloads) . '</div>';
  if ($items_per_page > 0) {
    $output .= theme('pager', array('tags' => array()));
  }
  if (!empty($page_footer['value'])) {
    $output .= '<div id="download-count-footer">' . check_markup($page_footer['value'], $page_footer['format']) . '</div>';
  }
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
      '#value' => check_plain($dc_entry->filename),
    );
    $form['fid'] = array(
      '#type' => 'value',
      '#value' => $dc_entry->fid,
    );
    $form['type'] = array(
      '#type' => 'value',
      '#value' => check_plain($dc_entry->type),
    );
    $form['id'] = array(
      '#type' => 'value',
      '#value' => $dc_entry->id,
    );
    return confirm_form(
      $form,
      t('Are you sure you want to reset the download count for %filename on %entity #%id?', array('%filename' => $dc_entry->filename, '%entity' => $dc_entry->type, '%id' => $dc_entry->id)),
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

    $result = db_truncate('download_count')->execute();
    if ($result) {
      db_truncate('download_count_cache')->execute();
      backdrop_set_message(t('All download counts have been reset.'));
      watchdog('download_count', 'All download counts have been reset.', array(), WATCHDOG_NOTICE);
    }
    else {
      backdrop_set_message(t('Unable to reset all download counts.'), 'error');
      watchdog('download_count', 'Unable to reset all download counts.', array(), WATCHDOG_ERROR);
    }
  }
  else {
    $result = db_delete('download_count')
      ->condition('fid', $form['fid']['#value'])
      ->condition('type', $form['type']['#value'])
      ->condition('id', $form['id']['#value'])
      ->execute();
    if ($result) {
      db_delete('download_count_cache')
        ->condition('fid', $form['fid']['#value'])
        ->condition('type', $form['type']['#value'])
        ->condition('id', $form['id']['#value'])
        ->execute();
      backdrop_set_message(t('Download count for %filename on %type %id was reset.', array('%filename' => $form['filename']['#value'], '%type' => $form['type']['#value'], '%id' => $form['id']['#value'])));
      watchdog('download_count', 'Download count for %filename on %type %id was reset.', array('%filename' => $form['filename']['#value'], '%type' => $form['type']['#value'], '%id' => $form['id']['#value']), WATCHDOG_NOTICE);
    }
    else {
      backdrop_set_message(t('Unable to reset download count for %filename on %type %id.', array('%filename' => $form['filename']['#value'], '%type' => $form['type']['#value'], '%id' => $form['id']['#value'])), 'error');
      watchdog('download_count', 'Unable to reset download count for %filename on %type %id.', array('%filename' => $form['filename']['#value'], '%type' => $form['type']['#value'], '%id' => $form['id']['#value']), WATCHDOG_ERROR);
    }
  }
  $form_state['redirect'] = 'admin/reports/download-count';
}

/**
 * Implements page callback for download count details.
 */
function download_count_view_details($dc_entry = NULL) {
  $config = config('download_count.settings');
  backdrop_add_css(backdrop_get_path('module', 'download_count') . '/download_count.css');

  // Set page title
  backdrop_set_title($dc_entry == 'all'
    ? t('Download Count Details - All Files')
    : t('Download Count Details - @filename from @type @id', array('@filename' => $dc_entry->filename, '@type' => $dc_entry->type, '@id' => $dc_entry->id))
  );

  // Ensure Peity is loaded before our chart script
  backdrop_add_js(backdrop_get_path('module', 'download_count') . '/js/peity-vanilla.min.js', 'file');
  backdrop_add_js(backdrop_get_path('module', 'download_count') . '/js/download_count_chart.js', 'file');

  $query = db_select('download_count_cache')
    ->fields(NULL, array('date'))
    ->groupBy('time_interval');
  $query->addExpression('SUM(count)', 'count');
  $query->orderBy('date', 'DESC');

  // Daily Data
  $query->addExpression("FROM_UNIXTIME(date, '%Y-%m-%d')", 'time_interval');
  $query->range(0, $config->get('download_count_details_daily_limit'));
  $result = $query->execute();
  $daily = _download_count_details_table($result, 'Daily', 'Day');

  if (!is_array($daily) || !isset($daily['values'])) {
    watchdog('download_count', 'Unexpected return value from _download_count_details_table()', [], WATCHDOG_WARNING);
    $daily['values'] = []; // Ensure it's an array to prevent further errors
  }

  $output = $daily['output'];
  $values['daily'] = !empty($daily['values']) ? implode(',', array_reverse($daily['values'])) : '0';
  $output .= '<div class="peity-data" data-values="' . $values['daily'] . '"></div>';

  // Weekly Data
  $query->addExpression("FROM_UNIXTIME(date, '%U')", 'time_interval');
  $query->range(0, $config->get('download_count_details_weekly_limit'));
  $result = $query->execute();
  $weekly = _download_count_details_table($result, 'Weekly', 'Week');
  $output .= $weekly['output'];
  $values['weekly'] = implode(',', array_reverse($weekly['values']));
  $output .= '<div class="peity-data" data-values="' . $values['weekly'] . '"></div>';

  // Monthly Data
  $query->addExpression("FROM_UNIXTIME(date, '%Y-%m')", 'time_interval');
  $query->range(0, $config->get('download_count_details_monthly_limit'));
  $result = $query->execute();
  $monthly = _download_count_details_table($result, 'Monthly', 'Month');
  $output .= $monthly['output'];
  $values['monthly'] = implode(',', array_reverse($monthly['values']));
  $output .= '<div class="peity-data" data-values="' . $values['monthly'] . '"></div>';

  return $output;
}

/**
 * Create and output details table.
 */
function _download_count_details_table($result, $caption, $range) {
  $header = array(
    array(
      'data' => t('#'),
      'class' => 'number',
    ),
    array(
      'data' => t($range),
      'class' => 'range',
    ),
    array(
      'data' => t('Downloads'),
      'class' => 'count',
    ),
  );

  $count = 1;
  $rows = array();
  $values = array(); // Ensure it is always defined

  foreach ($result as $download) {
    $row = array();
    $row[] = $count;
    $row[] = $download->time_interval;
    $row[] = number_format($download->count);
    $values[] = $download->count;
    $rows[] = $row;
    $count++;
  }

  $output = theme('table', array(
    'header' => $header,
    'rows' => $rows,
    'caption' => $caption,
    'sticky' => FALSE,
    'attributes' => array(
      'id' => 'download-count-' . backdrop_strtolower(backdrop_clean_css_identifier($caption)),
      'class' => 'download-count-details download-count-table'
    ),
  ));

  return array('output' => $output, 'values' => $values);
}

