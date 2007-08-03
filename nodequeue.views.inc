<?php
// $Id$

/**
 * @file nodequeue.views.inc
 * Provides support for the Views module.
 */

/**
 * Implementation of hook_views_tables()
 */
function nodequeue_views_tables() {
  $tables['nodequeue_nodes'] = array(
    "name" => "nodequeue_nodes",
    "join" => array(
      "left" => array(
        "table" => "node",
        "field" => "nid"
      ),
      "right" => array(
        "field" => "nid"
      ),
    ),
    "fields" => array(
      'timestamp' => array(
        'name' => t('NodeQueue: Timestamp'),
        'sortable' => true,
        'handler' => views_handler_field_dates(),
        'option' => 'string',
        'help' => t('Display the time the node was added to a given node queue.').' '.
                  t('The option field may be used to specify the custom date format as it\'s required by the date() function or if "as time ago" has been chosen to customize the granularity of the time interval.'),
      ),
    ),
    "filters" => array(
      "sqid" => array(
        'name' => t("NodeQueue: Queue"),
        'list' => 'nodequeue_handler_queuelist',
        'operator' => 'views_handler_operator_andor',
        'help' => t('Filter the view to a specific Node Queue. This only applies to queues with a single subqueue.'),
      ),
      "position" => array(
        'name' => t("NodeQueue: Queue Position"),
        'field' => 'position',
        'operator' => 'views_handler_operator_gtlt',
        'help' => t('Filter by where in the queue an item is.'),
      ),
      "timestamp" => array(
        'name' => t('NodeQueue: Queue Timestamp'),
        'operator' => 'views_handler_operator_gtlt',
        'value' => views_handler_filter_date_value_form(),
        'handler' => 'views_handler_filter_timestamp',
        'option' => 'string',
        'help' => t('This filter allows nodes to be filtered by the time they were added to a given NodeQueue.') .' '. views_t_strings('filter date'),
      ),
    ),
    "sorts" => array(
      "position" => array(
        'name' => t("NodeQueue: Queue Position"),
        'field' => 'position',
        'help' => t('When sorting by queue position, be sure the view is filtered to a single queue or the sort will not work very well.'),
      ),
      'timestamp' => array(
        'name' => t('NodeQueue: Timestamp'),
        'handler' => 'views_handler_sort_date',
        'option' => views_handler_sort_date_options(),
        'help' => t('Sort by the time the node was added to a node queue.'),
      ),
    ),
  );

  $queues = nodequeue_load_queues(nodequeue_get_all_qids(NULL));
  foreach ($queues as $queue) {
    if (count($queue->subqueue) > 1) {
      $tables['nodequeue_nodes']['filters']["qid_$queue->qid"] = array(
        'name' => t("NodeQueue: Subqueues for @queue", array('@queue' => $queue->title)),
        'list' => 'nodequeue_handler_subqueuelist',
        'qid' => $queue->qid,
        'operator' => 'views_handler_operator_andor',
        'help' => t('Filter the view to a specific Node Queue.'),
      );
    }
  }
  $tables['nodequeue_subqueue'] = array(
    "name" => "nodequeue_subqueue",
    "join" => array(
      "left" => array(
        "table" => "nodequeue_nodes",
        "field" => "sqid"
      ),
      "right" => array(
        "field" => "sqid"
      ),
    ),
  );
  $tables['nodequeue_queue'] = array(
    "name" => "nodequeue_queue",
    "join" => array(
      "left" => array(
        "table" => "nodequeue_subqueue",
        "field" => "qid"
      ),
      "right" => array(
        "field" => "qid"
      ),
    ),
  );
  return $tables;
}

function nodequeue_views_arguments() {
  $arguments = array(
    'nodequeue_qid' => array(
      'name' => t("NodeQueue: Queue ID"),
      'handler' => "nodequeue_handler_arg_qid",
      'help' => t('The Queue ID argument allows users to filter a view by specifying a Node Queue ID. This is only valid for simple nodequeues with just one subqueue.'),
    ),
    'nodequeue_qtitle' => array(
      'name' => t("NodeQueue: Queue Title"),
      'handler' => "nodequeue_handler_arg_qtitle",
      'help' => t('The Queue Title argument allows users to filter a view by specifying a Node Queue Title. This is only valid for simple nodequeues with just one subqueue.'),
    ),
  );
  $arguments["nodequeue_sqid"] = array(
    'name' => t("NodeQueue: Subqueue ID"),
    'handler' => "nodequeue_handler_arg_sqid",
    'operator' => 'views_handler_operator_andor',
    'help' => t('Filter the view to a specific Sub Queue.'),
  );
  $arguments["nodequeue_reference"] = array(
    'name' => t("NodeQueue: Subqueue reference"),
    'handler' => "nodequeue_handler_arg_reference",
    'operator' => 'views_handler_operator_andor',
    'option' => 'nodequeue_handler_queuelist_all',
    'help' => t('Filter the view to a specific Sub Queue. The subqueue reference is the ID that the subqueue is attached to; tax id for taxonomy or user id for user queues, etc.'),
  );
  return $arguments;
}

function nodequeue_views_default_views() {
  $queues = nodequeue_load_queues(nodequeue_get_all_qids(NULL));
  foreach ($queues as $queue) {
    $view = new stdClass();
    $view->name = "nodequeue_$queue->qid";
    $view->disabled = TRUE;
    $view->description = t('View node queue @queue', array('@queue' => $queue->title));
    $view->access = array();
    $view->view_args_php = '';
    $view->page = TRUE;
    $view->page_title = check_plain($queue->title);
    $view->page_header = '';
    $view->page_header_format = '1';
    $view->page_footer = '';
    $view->page_footer_format = '1';
    $view->page_empty = '';
    $view->page_empty_format = '1';
    $view->page_type = 'teaser';
    $view->url = 'nodequeue/' . $queue->qid;
    $view->use_pager = TRUE;
    $view->nodes_per_page = '10';
    $view->block = TRUE;
    $view->block_title = check_plain($queue->title);
    $view->block_header = '';
    $view->block_header_format = '1';
    $view->block_footer = '';
    $view->block_footer_format = '1';
    $view->block_empty = '';
    $view->block_empty_format = '1';
    $view->block_type = 'list';
    $view->nodes_per_block = '5';
    $view->block_more = TRUE;
    $view->block_use_page_header = FALSE;
    $view->block_use_page_footer = FALSE;
    $view->block_use_page_empty = FALSE;
    $view->sort = array (
      array (
        'tablename' => 'nodequeue_nodes',
        'field' => 'position',
        'sortorder' => 'ASC',
        'options' => '',
      ),
    );
    $view->argument = array (
    );
    $view->field = array (
      array (
        'tablename' => 'node',
        'field' => 'title',
        'label' => '',
        'handler' => 'views_handler_field_nodelink',
        'options' => 'link',
      ),
    );
    $view->exposed_filter = array (
    );
    $view->requires = array('nodequeue_nodes', 'node');
    if (count($queue->subqueue) > 1) {
      // Add an argument to select a subqueue
    }
    else {
      $view->filter = array (
        array (
          'tablename' => 'nodequeue_nodes',
          'field' => 'sqid',
          'operator' => 'OR',
          'options' => '',
          'value' => array($queue->current),
        ),
      );
    }
    $views[$view->name] = $view;
  }
  return $views;
}

function nodequeue_handler_queuelist() {
  $queues = nodequeue_load_queues(nodequeue_get_all_qids(NULL));
  foreach ($queues as $queue) {
    if (count($queue->subqueue) == 1) {
      $items[$queue->current] = $queue->title;
    }
  }
  return $items;
}

function nodequeue_handler_queuelist_all() {
  $queues = nodequeue_load_queues(nodequeue_get_all_qids(NULL));
  foreach ($queues as $queue) {
    $items[$queue->qid] = $queue->title;
  }
  return $items;
}

function nodequeue_handler_arg_qid($op, &$query, $argtype, $arg = '') {
  switch($op) {
    case 'summary':
      $query->ensure_table('nodequeue_queue', true);
      $query->add_field('title', 'nodequeue_queue');
      $query->add_field('qid', 'nodequeue_queue');
      $query->add_where('nodequeue_queue.qid IS NOT NULL');
      $fieldinfo['field'] = "nodequeue_queue.title";
      return $fieldinfo;
      break;
    case 'sort':
      $query->add_orderby('nodequeue_queue', 'title', $argtype);
      break;
    case 'filter':
      $qid = intval($arg);
      $query->ensure_table('nodequeue_queue', true);
      $query->add_where("nodequeue_queue.qid = %d", $qid);
      break;
    case 'link':
      return l($query->title, "$arg/" . intval($query->qid));
    case 'title':
      $queue = db_fetch_object(db_query("SELECT title FROM {nodequeue_queue} WHERE qid = %d", $query));
      return $queue->title;
  }
}

function nodequeue_handler_arg_qtitle($op, &$query, $argtype, $arg = '') {
  switch($op) {
    case 'summary':
      $query->ensure_table('nodequeue_queue', true);
      $query->add_field('title', 'nodequeue_queue');
      $query->add_field('qid', 'nodequeue_queue');
      $query->add_where('nodequeue_queue.qid IS NOT NULL');
      $fieldinfo['field'] = "nodequeue_queue.title";
      return $fieldinfo;
      break;
    case 'sort':
      $query->add_orderby('nodequeue_queue', 'title', $argtype);
      break;
    case 'filter':
      $qtitle = $arg;
      $query->ensure_table('nodequeue_queue', true);
      $query->add_where("nodequeue_queue.title = '%s'", $qtitle);
      break;
    case 'link':
      return l($query->title, "$arg/" . intval($query->qid));
    case 'title':
      return check_plain($query);
  }
}

function nodequeue_handler_arg_sqid($op, &$query, $argtype, $arg = '') {
  switch($op) {
    case 'summary':
      $query->ensure_table('nodequeue_subqueue', true);
      $query->add_field('title', 'nodequeue_subqueue');
      $query->add_field('sqid', 'nodequeue_subqueue');
      $query->add_where('nodequeue_subqueue.sqid IS NOT NULL');
      $fieldinfo['field'] = "nodequeue_queue.title";
      return $fieldinfo;
      break;
    case 'sort':
      $query->add_orderby('nodequeue_queue', 'title', $argtype);
      break;
    case 'filter':
      $qid = intval($arg);
      $query->ensure_table('nodequeue_queue', true);
      $query->add_where("nodequeue_queue.qid = %d", $qid);
      break;
    case 'link':
      return l($query->title, "$arg/" . intval($query->qid));
    case 'title':
      $queue = db_fetch_object(db_query("SELECT title FROM {nodequeue_queue} WHERE qid = %d", $query));
      return $queue->title;
  }
}
