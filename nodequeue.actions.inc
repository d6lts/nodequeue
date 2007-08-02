<?php
// $Id$

/**
 * @file nodequeue.actions.inc
 * Provides actions integration for node queues.
 */

/**
 * Action to add a node to a queue.
 */
function action_nodequeue_add($op, $edit = array(), $node) {
  switch($op) {
    case 'metadata':
      return array(
        'description' => t('Add to Node Queue'),
        'type' => t('node'),
        'batchable' => true,
        'configurable' => true,
      );
      break;

    case 'do':
      $qid = $edit['qid'];
      nodequeue_queue_add($qid, $node->nid);
      break;

    // return an HTML config form for the action
    case 'form':
      // default values for form
      if (!isset($edit['qid'])) $edit['qid'] = '';

      $result = db_query("SELECT * from {nodequeue_queue} ORDER BY title");
      while ($obj = db_fetch_object($result))
        $queues[$obj->qid] = $obj->title;

      // add form components
      $form['qid'] = array(
        '#type' => 'select',
        '#title' => t("Queue"),
        '#default_value' => $edit['qid'],
        '#options' => $queues,
      );
      return $form;

     // validate the HTML form

    // process the HTML form to store configuration
    case 'submit':
      $params = array(
        'qid' => $edit['qid']
      );
      return $params;
      break;
  }
}

/**
 * Action to remove a node from a queue.
 */
function action_nodequeue_remove($op, $edit = array(), $node) {
  switch($op) {
    case 'metadata':
      return array(
        'description' => t('Remove from Node Queue'),
        'type' => t('node'),
        'batchable' => true,
        'configurable' => true,
      );
      break;

    case 'do':
      $qid = $edit['qid'];
      nodequeue_queue_remove_node($qid, $node->nid);
      break;

    // return an HTML config form for the action
    case 'form':
      // default values for form
      if (!isset($edit['qid'])) $edit['qid'] = '';

      $result = db_query("SELECT * from {nodequeue_queue} ORDER BY title");
      while ($obj = db_fetch_object($result))
        $queues[$obj->qid] = $obj->title;

      // add form components
      $form['qid'] = array(
        '#type' => 'select',
        '#title' => t("Queue"),
        '#default_value' => $edit['qid'],
        '#options' => $queues,
      );
      return $form;
      break;

     // validate the HTML form

    // process the HTML form to store configuration
    case 'submit':
      $params = array(
        'qid' => $edit['qid']
      );
      return $params;
      break;
  }
}
