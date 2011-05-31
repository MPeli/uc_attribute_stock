<?php
/**
 * Display a list of discounts.
 */
function uc_discounts_admin_list() {
  $search_params = array(
    'name' => arg(4),
  );

  $header = array(
    array('data' => t('Active'), 'field' => 'is_active'),
    array('data' => t('Name'), 'field' => 'name'),
    array('data' => t('Short Description'), 'field' => 'short_description'),
    array('data' => t('Qualifying Type'), 'field' => 'qualifying_type'),
    array('data' => t('Type'), 'field' => 'discount_type'),
    array('data' => t('Amount'), 'field' => 'discount_amount'),
    array('data' => t('Weight'), 'field' => 'weight'),
    array('data' => t('Created At'), 'field' => 'insert_timestamp', 'sort' => 'desc'),
    array('data' => t('Expires At'), 'field' => 'expiration'),
    array('data' => t('Operations')),
  );

	$select = db_select('uc_discounts', 'd');
  if (!empty($search_params['name'])) {
    $name = db_escape_string('%' . $search_params['name'] . '%');
		$arguments = array(':name' => $name);
		$result->condition('WHERE name LIKE :name OR description LIKE :name OR short_description LIKE :name', $arguments);
  }
	
	$select->fields('d');
  $select->extend('PagerDefault')->limit(10)->extend('TableSort')->orderByHeader($header);
	$result = $select->execute()->fetchAll();
	

  $rows = array();
	/*
  foreach ($result as $discount) {
    $total_use_count = (is_numeric($discount->total_use_count)) ? $discount->total_use_count : 0;
    $total_times_applied = (is_numeric($discount->total_times_applied)) ? $discount->total_times_applied : 0;
    $operations = array(
      l(t("usage"), "admin/reports/uc_discounts/discount/". $discount->discount_id),
      l(t("edit"), "admin/store/uc_discounts/edit/". $discount->discount_id),
      l(t("copy"), "admin/store/uc_discounts/copy/". $discount->discount_id),
      l(t("delete"), "admin/store/uc_discounts/delete/". $discount->discount_id),
    );

    $rows[] = array('data' =>
      array(
        array('data' => ($discount->is_active ? '&#10003;' : 'X'), 'class' => ($discount->is_active ? 'is_active' : 'is_inactive')),
        $discount->name,
        $discount->short_description,
        qualifying_type_name($discount->qualifying_type),
        discount_type_name($discount->discount_type),
        array('data' => discount_amount_formatted($discount), 'class' => 'numeric'),
        array('data' => $discount->weight, 'class' => 'numeric'),
        array('data' => format_date($discount->insert_timestamp, 'small'), 'class' => 'date'),
        array('data' => ($discount->has_expiration ? format_date($discount->expiration, 'small') : 'N/A'), 'class' => 'date'),
        array('data' => implode(' ', $operations), 'class' => 'operations'),
      ),
    );
  }
	*/
	$rows = array();
  if (empty($rows)) {
    $rows[] = array(array('data' => t('No discounts.'), 'colspan' => count($header)));
  }

/*
  $output = drupal_get_form('uc_discounts_search_form', $search_params);
  $output .= theme('table', $header, $rows, array('class' => 'uc-discounts'));
  $output .= theme('pager', NULL, 50, 0);
  return $output;
*/

	$header = array('A');
	$rows = array(
      array(
        'data' =>'Cell 1'
      ),
      array(
        'data' =>'Cell 1'
      ),
    );
  $output .= theme('table', array('header' => $header, 'rows' => $rows));
	
	print_r($rows);

  $output = drupal_get_form('uc_discounts_search_form', $search_params);
  #$output .= theme('table', array('header' => $header, 'rows' => $rows, 'attributes' => array('class' => 'uc-discounts')));
  return $output;
	
}


/**
 * Create a form for searching the discounts
 */
function uc_discounts_search_form($form, &$form_state, $values) {
  $form = array();

  $form['search'] = array(
    '#type' => 'fieldset',
    '#title' => t('Search discounts'),
    '#collapsible' => TRUE,
    '#collapsed' => empty($values['name']),
  );

  $form['search']['name'] = array(
    '#type' => 'textfield',
    '#title' => t('Description/Name'),
    '#default_value' => $values['name'],
  );

  $form['search']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Search'),
  );

  return $form;
}