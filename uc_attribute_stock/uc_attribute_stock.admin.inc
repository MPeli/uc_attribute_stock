<?php
  
function uc_attribute_stock_ajax($nid) {
  $data = get_attributes($nid);
  #print_r($data);
  // Return json
  drupal_json_output($data);
  
}


function uc_attribute_stock_form($form, &$form_state, $node) {
  drupal_set_title($node->title);
  $nid = $node->nid;

  // Populate table and such.
  $model = $node->model;
  $query_select = "SELECT DISTINCT";
  $query_from = " FROM";
  $query_where = " WHERE";
  $query_order = " ORDER BY";
  $result = db_query("SELECT pa.nid, pa.aid, pa.ordering, pa.display, a.name, a.ordering, ao.aid, COUNT(po.oid) FROM {uc_product_attributes} AS pa LEFT JOIN {uc_attributes} AS a ON pa.aid = a.aid LEFT JOIN {uc_attribute_options} AS ao ON a.aid = ao.aid LEFT JOIN {uc_product_options} AS po ON ao.oid = po.oid AND po.nid = :po_nid WHERE pa.nid = :pa_nid AND pa.display <> :pa_display GROUP BY ao.aid, pa.aid, pa.display, a.name, pa.ordering, a.ordering, pa.nid HAVING count(po.oid) > 0 ORDER BY pa.ordering, a.ordering", array(':po_nid' => $nid, ':pa_nid' => $nid, ':pa_display' => 3));

  $i = 1;
  $attribute_names = '';
  $full_attributes = array();
  $values = array();

  $query = db_select('uc_product_options', "upo");
  
  $query->leftJoin('uc_attribute_options', "uao", "upo.oid = uao.oid AND upo.nid = :nid", array(':nid' => $nid));
  $query->addField('uao', 'aid', 'aid');
  $query->addField('uao', 'name', 'name');
  $query->addField('uao', 'oid', 'oid');
  $query->addField('upo', 'ordering', 'ordering');

  $query->condition("upo.nid", $nid)
    ->orderBy("upo.ordering")
    ->orderBy("uao.name");
    

  $attribute_ids = array();
  foreach ($result as $prod_attr) {

    $attribute_names .= '<th>' . check_plain($prod_attr->name) . '</th>';
    $attribute_ids[] = $prod_attr->aid;
  }
  
  $num_prod_attr = count($attribute_ids);

  if ($num_prod_attr) {    
    
    $form['original'] = array(
      '#markup' => '<p><b>' . t('Default product SKU: @sku', array('@sku' => $model)) . '</b></p>',
    );
    $form['default'] = array(
      '#type' => 'value',
      '#value' => 0,
    );
    $form['table'] = array(
      '#prefix' => '<table class="combinations">',
      '#suffix' => '</table>',
    );
    $form['table']['head'] = array(
      '#markup' => '<thead><tr><th>' . t('Active') . '</th>' . $attribute_names . '<th>' . t('Stock') . '</th><th>' . t('Treshold') . '</th></tr></thead>',
      '#weight' => 0,
    );
    $form['table']['body'] = array(
      '#prefix' => '<tbody>',
      '#suffix' => '</tbody>',
      '#weight' => 1,
      '#tree' => TRUE,
    );
    
    $result = $query->execute();
    
    while ($combo = $result->fetchObject()) {
      $options[$combo->aid][] = array('name' => $combo->name, 'oid' => $combo->oid);

    }
    ksort($options);
    
    $old_vals = db_query("(SELECT * FROM {uc_attribute_stock} AS uas WHERE uas.nid = :nid)", array(':nid' => $nid))->fetchAll();
    
    $i = 0;
    foreach ($old_vals as $ov) {
      $combination = unserialize($ov->combination);
      $form['table']['body'][$i] = array(
        '#prefix' => '<tr>',
        '#suffix' => '</tr>',
      );
      $form['table']['body'][$i]['active'] = array(
        '#type' => 'checkbox',
        '#default_value' => 1,
        '#prefix' => '<td>',
        '#suffix' => '</td>',
      );

      
      foreach($options as $key => $value) {
        $select_options[''] = t('Please select');
        $aid = $key;
        foreach($value as $k => $v) {
          $select_options[$v['oid']] = t($v['name']);
        }
        $form['table']['body'][$i]['atributes'][$aid] = array(
          '#type' => 'select',
          '#default_value' => $combination[$aid],
          '#options' => $select_options,
          '#description' => t('The log.'),
          '#prefix' => '<td>',
          '#suffix' => '</td>',
        );
        
        $select_options = '';
      
      }
      
      $form['table']['body'][$i]['uas_id'] = array(
        '#type' => 'value',
        '#value' => $ov->id,
      );

      
      $form['table']['body'][$i]['stock'] = array(
        '#type' => 'textfield',
        '#default_value' => $ov->stock,
        '#prefix' => '<td>',
        '#suffix' => '</td>',
        '#size' => 5,
      );
      $form['table']['body'][$i]['treshold'] = array(
        '#type' => 'textfield',
        '#default_value' => $ov->treshold,
        '#prefix' => '<td>',
        '#suffix' => '</td>',
        '#size' => 5,
      );
      $i++;
    }


    $x = $i + 10;
    for($i; $i < $x; $i++) {
      $form['table']['body'][$i] = array(
        '#prefix' => '<tr>',
        '#suffix' => '</tr>',
      );
      $form['table']['body'][$i]['active'] = array(
        '#type' => 'checkbox',
        '#default_value' => '',
        '#prefix' => '<td>',
        '#suffix' => '</td>',
      );

      
      foreach($options as $key => $value) {
        $select_options[''] = t('Please select');
        $aid = $key;
        foreach($value as $k => $v) {
          $select_options[$v['oid']] = t($v['name']);
        }
        
        $form['table']['body'][$i]['atributes'][$aid] = array(
          '#type' => 'select',
          '#options' => $select_options,
          '#description' => t('The log.'),
          '#prefix' => '<td>',
          '#suffix' => '</td>',
        );
        
        $select_options = '';
      
      }
      
      $form['table']['body'][$i]['uas_id'] = array(
        '#type' => 'value',
        '#value' => 0,
      );

      
      $form['table']['body'][$i]['stock'] = array(
        '#type' => 'textfield',
        '#default_value' => 0,
        '#prefix' => '<td>',
        '#suffix' => '</td>',
        '#size' => 5,
      );
      $form['table']['body'][$i]['treshold'] = array(
        '#type' => 'textfield',
        '#default_value' => 0,
        '#prefix' => '<td>',
        '#suffix' => '</td>',
        '#size' => 5,
      );
    }
    

    $form['nid'] = array(
      '#type' => 'hidden',
      '#value' => $nid,
    );
    $form['actions'] = array('#type' => 'actions');
    $form['actions']['submit'] = array(
      '#type' => 'submit',
      '#value' => t('Submit'),
    );
  }
  else {
    $form['error'] = array(
      '#markup' => '<div><br />' . t('This product does not have any attributes that can be used for SKU adjustments.') . '</div>',
    );
  }

  $form['pager'] = array(
    '#theme' => 'pager',
  );

  return $form;
}

function theme_uc_attribute_stock_form($variables) {

}

/**
 * Form builder for uc_attribute_stock_form().
 *
 * @see uc_attribute_stock_form()
 */
function uc_attribute_stock_form_submit($form, &$form_state) {
  
  
  foreach ($form_state['values']['body'] as $value) {
    
    foreach($value['atributes'] as $aid => $oid) {
        $comb_array[$aid] = $oid;
    }
    $combination = serialize($comb_array);
    
    if($value['uas_id']) {
      if ($value['active'] == 1) {
        $val = db_query("SELECT * FROM {uc_attribute_stock} AS uac WHERE combination = :comb AND id != :id", array(':comb' => $combination, ':id' => $value['uas_id']))->fetchAll();
        
        if(count($val) == 0) {
          db_update('uc_attribute_stock')
            ->fields(array(
              'combination' => $combination,
              'active' => $value['active'],
              'stock' => $value['stock'],
              'treshold' => $value['treshold'],
            ))
            ->condition('id', $value['uas_id'], '=')
            ->execute();
        }
        else {
          drupal_set_message(t('The combination already exists!'), 'error');
        }
      }
      else {
        db_delete('uc_attribute_stock')
          ->condition('id', $value['uas_id'])
          ->execute();
      }
    }
    else {
      if ($value['active'] == 1) {
        db_merge('uc_attribute_stock')
          ->key(array(
            'nid' => $form_state['values']['nid'],
            'combination' => $combination,
          ))
          ->fields(array(
            'active' => $value['active'],
            'stock' => $value['stock'],
            'treshold' => $value['treshold'],
          ))
          ->execute();
      }
    }
  }  
  drupal_set_message(t('Product adjustments have been saved.'));
}