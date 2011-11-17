<?php

/**
 * @file
 * Rules specific functions for the download_count module.
 */

/**
 * Implements hook_rules_event_info().
 */
function download_count_rules_event_info() {
  return array(
    'download_count_file_download' => array(
      'label' => t('A file has been downloaded'),
      'module' => 'Download Count',
      'variables' => array(
        'filename' => array(
          'type' => 'file',
          'label' => t('Filename of the downloaded file.'),
        ),
        'user' => array(
          'type' => 'file',
          'label' => t('User object of the use who downloaded the file.'),
        ),
        'entity_type' => array(
          'type' => 'file',
          'label' => t('Entity type of the entity from which the file was downloaded.'),
        ),
        'entity_id' => array(
          'type' => 'file',
          'label' => t('ID of the entity from which the file was downloaded.'),
        ),
        'ip' => array(
          'type' => 'file',
          'label' => t('IP address of the user who downloaded the file.'),
        ),
        'referrer' => array(
          'type' => 'file',
          'label' => t('HTTP Referrer of the downloaded file.'),
        ),
        'time' => array(
          'type' => 'file',
          'label' => t('Timestamp of the download.'),
        ),
      ),
    ),
  );
}
