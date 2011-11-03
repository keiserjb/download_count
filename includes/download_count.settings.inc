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
    '#description' => t('To exclude files of certain types, enter the extensions to exclude separated by spaces. This is useful if you have private image fields and don\'t wish to count their downloads.'),
  );
  $form['download count page'] = array(
    '#type' => 'fieldset',
    '#title' => t('Download count page settings'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['download count page']['download_count_view_page_limit'] = array(
    '#type' => 'textfield',
    '#title' => t('Total number of items to display'),
    '#size' => 10,
    '#weight' => -1,
    '#default_value' => variable_get('download_count_view_page_limit', 0),
    '#description' => t('Set to 0 for no limit.'),
  );
  $form['download count page']['download_count_view_page_items'] = array(
    '#type' => 'textfield',
    '#title' => t('Number of items per page.'),
    '#size' => 10,
    '#weight' => -1,
    '#default_value' => variable_get('download_count_view_page_items', 0),
    '#description' => t('Set to 0 for no pager.'),
  );
   $form['download count page']['download_count_view_page_title'] = array(
    '#type' => 'textfield',
    '#title' => t('Title'),
    '#default_value' => variable_get('download_count_view_page_title', t('Download Count')),
    '#description' => t('Title of this <a href="@page">page</a>.', array('@page' => url('download_count'))),
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

  return system_settings_form($form);
}
