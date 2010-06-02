<?php
// $Id$

/**
 * @file
 * Administrative page callbacks for the download_count module.
 */

/**
 * Download_count administration settings.
 *
 * @return
 * Form for download_count configuration options.
 */
function download_count_admin_settings() {
  switch (variable_get('file_downloads', FILE_DOWNLOADS_PUBLIC)) {
    case FILE_DOWNLOADS_PUBLIC:
      $output = '<p>' . t('You are using public download. Set your download method to private on the File system configuration <a href="@admin-file-system">page</a>, if you wish for Drupal to keep a record of downloaded files.', array('@admin-file-system' => url('admin/settings/file-system'))) . '</p>';
      break;
    case FILE_DOWNLOADS_PRIVATE:
      $output = '<p>' . t('You are using the private download method, hence you have the correct setting to allow this module to keep a record of downloaded files.') . '</p>';
      break;
  }
  $form['info on download method'] = array(
    '#value' => $output,
  );
  $form['ignoring a set of file extensions'] = array(
    '#type' => 'fieldset',
    '#title' => t('Ignoring certain file extensions'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );
  $form['ignoring a set of file extensions']['download_count_excluded_file_extensions'] = array(
    '#type' => 'textfield',
    '#title' => t('Excluded file extensions'),
    '#default_value' => variable_get('download_count_excluded_file_extensions', 'jpg jpeg gif png'),
    '#maxlength' => 255,
    '#description' => t('This module only considers files that have been uploaded with the upload module and that become file attachments.  However, if you are using a contributed module to upload images and display them in the body of nodes, these files may get flagged as downloaded whenever a visitor or robot views the node page, because strictly speaking they are file attachments. This will happen with the module img_assist. Note that this won\'t happen with the module imce, because imce treats inline images as nodes. If you do not want to set a download count for image files, list their extension here. Separate extensions with a space and do not include the leading dot. For example, you could list these extensions&nbsp: jpg jpeg gif png. If you do not want to exclude any file, leave that field blank. ')
  );
  $form['dounload count in nodes'] = array(
    '#type' => 'fieldset',
    '#title' => t('Download count in nodes'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );
  $form['dounload count in nodes']['do_not_show_download_count_in_node_for_admin'] = array(
    '#type' => 'checkbox',
    '#title' => t('Do not show download count in the node view for the administrator'),
    '#default_value' => variable_get('do_not_show_download_count_in_node_for_admin', FALSE),
    '#description' => t('Check this if you do not want to see the download count in the node view.'),
  );
  $form['download count page'] = array(
    '#type' => 'fieldset',
    '#title' => t('Download count page'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );
  $form['download count page']['download_count_view_page_title'] = array(
    '#type' => 'textfield',
    '#title' => t('Title'),
    '#default_value' => variable_get('download_count_view_page_title', t('Download Count')),
    '#description' => t('Title of this <a href="@page">page</a>.', array('@page' => url('download_count'))),
  );
  $form['download count page']['download_count_view_page_header'] = array(
    '#type' => 'textarea',
    '#title' => t('Header'),
    '#cols' => 60,
    '#rows' => 6,
    '#default_value' => variable_get('download_count_view_page_header', t('')),
    '#description' => t('Text to appear between the title of the page and the download count table.'),
  );
  $form['download count page']['download_count_view_page_footer'] = array(
    '#type' => 'textarea',
    '#title' => t('Footer'),
    '#cols' => 60,
    '#rows' => 6,
    '#default_value' => variable_get('download_count_view_page_footer', t('')),
    '#description' => t('Text to appear underneath the download count table.'),
  );
  $form['download count page']['download_count_view_page_format'] = filter_form(variable_get('download_count_view_page_format', 2), 0, array('download_count_view_page_format'));
  $form['info_on_access control'] = array(
    '#value' => '<p>' . t('Visit the Permissions <a href="@page">page</a> to allow roles to view download count in nodes and on the download count page.', array('@page' => url('admin/user/permissions'))) . '</p>'
  );
  return system_settings_form($form);
}
