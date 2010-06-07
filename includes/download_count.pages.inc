<?php
// $Id$

/**
 * @file
 * Page callback file for the download_count module.
 */
function download_count_view_page($arg = NULL) {

  global $user;
  $rows = array();
  $header = array();
  drupal_set_title(t(variable_get('download_count_view_page_title', 'Download Count')));
  $limit = (int) variable_get('download_count_view_page_limit', '');
  $file_links = variable_get('download_count_file_links', 1);

  $header[] = array('data' => t('Count'), 'field' => 'count', 'sort' => 'desc', 'class' => 'count');
  $header[] = array('data' => t('File'), 'field' => 'filename', 'class' => 'file');
  $header[] = array('data' => t('Page'), 'field' => 'title', 'class' => 'page');
  $header[] = array('data' => t('Last Download'), 'field' => 'last', 'class' => 'date');

  if ($arg == 'download_count') {
    if (!$limit) {
      $result = db_query("SELECT COUNT(dc.dcid) AS count, f.filename, MAX(dc.timestamp) AS last, dc.nid, f.filepath, n.title FROM {download_count} dc JOIN {files} f ON dc.fid = f.fid JOIN {node} n ON dc.nid = n.nid GROUP BY dc.nid, f.filename" . tablesort_sql($header));
    }
    else {
      $result = db_query("SELECT COUNT(dc.dcid) AS count, f.filename, MAX(dc.timestamp) AS last, dc.nid, f.filepath, n.title FROM {download_count} dc JOIN {files} f ON dc.fid = f.fid JOIN {node} n ON dc.nid = n.nid GROUP BY dc.nid, f.filename" . tablesort_sql($header) . " LIMIT %d", $limit);
    }
    $output = '<div id="download_count_page">';
  }
  elseif ($arg == 'my_download_count') {
    if (!$limit) {
      $result = db_query("SELECT COUNT(dc.dcid) AS count, f.filename, MAX(dc.timestamp) AS last, dc.nid, f.filepath, n.title FROM {download_count} dc JOIN {files} f ON dc.fid = f.fid JOIN {node} n ON dc.nid = n.nid WHERE dc.uid = %d GROUP BY dc.nid, f.filename" . tablesort_sql($header), $user->uid);
    }
    else {
      $result = db_query("SELECT COUNT(dc.dcid) AS count, f.filename, MAX(dc.timestamp) AS last, dc.nid, f.filepath n.title FROM {download_count} dc JOIN {files} f ON dc.fid = f.fid JOIN {node} n ON dc.nid = n.nid WHERE dc.uid = %d GROUP BY dc.nid, f.filename" . tablesort_sql($header) . " LIMIT %d", $user->uid, $limit);
    }
    $output = '<div id="my_download_count_page">';
  }

  $total_downloads = 0;
  $files = array();
  $nodes = array();
  while ($file = db_fetch_object($result)) {
    $row = array();
    $node = node_load($file->nid);
    $row[] = $file->count;
    $row[] = ($file_links && (user_access('view uploaded files') || _download_count_is_accessible_by_filefield($file->filepath))) ? l(t('@filename', array('@filename' => $file->filename)), file_create_url($file->filepath)) : $file->filename;
    $row[] = node_access('view', $node) ? l(t('@title', array('@title' => $node->title)), 'node/' . $node->nid) : check_plain($node->title);
    $row[] = t('@time ago', array('@time' => format_interval(time() - $file->last)));
    $rows[] = $row;

    $total_downloads += $file->count;
    $files[] = $file->filename;
    $nodes[] = $node->title;
  }

  if (empty($rows)) {
    $rows[] = array(array('data' => t('No files have been downloaded.'), 'colspan' => '4'));
  }

  $output .= check_markup(variable_get('download_count_view_page_header', ''), variable_get('download_count_view_page_format', 0), FALSE);
  $output .= '<p>' . t('Unique Pages: ') . count(array_unique($nodes)) . (', Unique Files: ') . count(array_unique($files)) . ', Total Downloads: ' . $total_downloads . '</p>';
  $output .= theme('table', $header, $rows, array('id' => 'download_count_table'));
  $output .= '<p>' . t('Unique Pages: ') . count(array_unique($nodes)) . (', Unique Files: ') . count(array_unique($files)) . ', Total Downloads: ' . $total_downloads . '</p>';
  $output .= check_markup(variable_get('download_count_view_page_footer', ''), variable_get('download_count_view_page_format', 0), FALSE);
  $output .= '</div>';

  return $output;
}