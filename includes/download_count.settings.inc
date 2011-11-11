<?php

/**
 * @file
 * Administrative page callbacks for the download_count module.
 */

/**
 * Download count administration settings.
 *
 * @return
 * Form for download_count configuration options.
 */
function download_count_admin_settings($form, &$form_state) {
  $form['excluded file extensions'] = array(
    '#type' => 'fieldset',
    '#title' => t('Excluded file extensions'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );
  $form['excluded file extensions']['download_count_excluded_file_extensions'] = array(
    '#type' => 'textfield',
    '#title' => t('Excluded file extensions'),
    '#default_value' => variable_get('download_count_excluded_file_extensions', 'jpg jpeg gif png'),
    '#maxlength' => 255,
    '#description' => t('To exclude files of certain types, enter the extensions to exclude separated by spaces. This is useful if you have private image fields and don\'t wish to include them in download counts.'),
  );
  $form['download count page'] = array(
    '#type' => 'fieldset',
    '#title' => t('Download count page settings'),
    '#description' => t('Settings for <a href="@page">this</a> page.', array('@page' => url('admin/reports/download-count'))),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['download count page']['download_count_view_page_title'] = array(
    '#type' => 'textfield',
    '#title' => t('Title'),
    '#default_value' => variable_get('download_count_view_page_title', t('Download Counts')),
  );
  $form['download count page']['download_count_view_page_limit'] = array(
    '#type' => 'textfield',
    '#title' => t('Total number of items to display'),
    '#size' => 10,
    '#default_value' => variable_get('download_count_view_page_limit', 0),
    '#description' => t('Set to 0 for no limit.'),
  );
  $form['download count page']['download_count_view_page_items'] = array(
    '#type' => 'textfield',
    '#title' => t('Number of items per page'),
    '#size' => 10,
    '#default_value' => variable_get('download_count_view_page_items', 25),
    '#description' => t('Set to 0 for no pager.'),
  );
  $header = variable_get('download_count_view_page_header', '');
  $form['download count page']['download_count_view_page_header'] = array(
    '#type' => 'text_format',
    '#title' => t('Header'),
    '#format' => isset($header['format']) ? $header['format'] : NULL,
    '#default_value' => isset($header['value']) ? $header['value'] : NULL,
    '#description' => t('Text to appear between the title of the page and the download count table.'),
  );
  $footer = variable_get('download_count_view_page_footer', '');
  $form['download count page']['download_count_view_page_footer'] = array(
    '#type' => 'text_format',
    '#title' => t('Footer'),
    '#format' => isset($footer['format']) ? $footer['format'] : NULL,
    '#default_value' => isset($footer['value']) ? $footer['value'] : NULL,
    '#description' => t('Text to appear underneath the download count table.'),
  );
  $form['download_count_cache_clear'] = array(
    '#type' => 'fieldset',
    '#title' => t('Clear Download Count Cache'),
    '#description' => t('This will permanently remove the download count cache data from the database. It will be rebuilt during drupal cron runs.'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['download_count_cache_clear']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Clear Cache'),
    '#submit' => array('download_count_clear_submit'),
  );
  return system_settings_form($form);
}

/**
 * Clear cache button submit handler.
 */
function download_count_clear_submit($form, &$form_state) {
  $form_state['redirect'] = 'admin/config/media/download-count/clear';
}

/**
 * Form to confirm clearing of the download_count_cache table.
 */
function download_count_clear_confirm($form, &$form_state) {
  return confirm_form(
    $form,
    t('Are you sure you want to clear the download count cache?'),
    'admin/config/media/download-count',
    t('This action cannot be undone.'),
    t('Clear Cache'),
    t('Cancel')
  );
}

/**
 * Form submit handler for download_count_clear_confirm().
 */
function download_count_clear_confirm_submit($form, &$form_state) {
  variable_set('download_count_last_cron', 0);
  db_truncate('download_count_cache')->execute();
  drupal_set_message(t('The download count cache has been cleared.'));
  watchdog('download_count', 'The download count cache has been cleared.', array(), WATCHDOG_NOTICE);
  $form_state['redirect'] = 'admin/config/media/download-count';
}
