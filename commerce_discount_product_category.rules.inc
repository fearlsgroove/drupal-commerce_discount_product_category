<?php

/**
 * Implements hook_rules_condition_info().
 */
function commerce_discount_product_category_rules_condition_info() {
  $inline_conditions = inline_conditions_get_info();
  $conditions = array();

  $conditions['commerce_discount_product_category_has_specified_terms'] = array(
    'label' => t('Line item product display contains specific terms ID'),
    'parameter' => array(
      'commerce_line_item' => array(
        'type' => 'commerce_line_item',
        'label' => t('Line item'),
        'description' => t('The line item.'),
        'wrapped' => TRUE,
      ),
      'terms' => array(
        'type' => 'text',
        'label' => t('Terms ID'),
        'description' => t('Enter a comma-separated list of term ID to compare against the passed product line item.'),
      ),
    ),
    'module' => 'commerce_discount_product_category',
    'group' => t('Commerce Line Item'),
    'callbacks' => array(
      'execute' => $inline_conditions['commerce_discount_product_category_has_specified_terms']['callbacks']['build'],
    ),
  );
  return $conditions;
}

/**
 * Build callback for commerce_product_has_specified_terms on product.
 *
 * @param EntityDrupalWrapper $wrapper
 *   Wrapped entity type given by the rule.
 * @param array $terms
 *   Values for the condition settings.
 *
 * @return bool
 *   True is condition is valid. false otherwise.
 */
function commerce_discount_product_category_has_specified_terms_build(EntityDrupalWrapper $wrapper, $terms) {
  if (!in_array('commerce_product', array_keys($wrapper->getPropertyInfo()))) {
    return FALSE;
  }

  static $cached_items = array();
  $type = $wrapper->type();
  $product_id = $wrapper->commerce_product->getIdentifier();
  $cache_key = "$type:$product_id";
  $terms_to_check = explode(', ', $terms);

  if (!isset($cached_items[$cache_key])) {
    $cached_items[$cache_key] = array();
    // Collect product reference fields from the display nodes, to query them.
    static $product_reference_fields = FALSE;
    if ($product_reference_fields === FALSE) {
      $product_reference_fields = array();
      foreach (field_info_field_map() as $field_name => $field_stub) {
        if ($field_stub['type'] == 'commerce_product_reference' && !empty($field_stub['bundles']['node'])) {

          // Not every product reference field signifies that this node is
          // 'a display node for this particular product'. We assume it is, if the
          // "Render fields from the referenced products when viewing this entity"
          // per-instance setting is true.
          $bundles = array();
          foreach ($field_stub['bundles']['node'] as $bundle) {
            $instance_info = field_info_instance('node', $field_name, $bundle);
            if (!empty($instance_info['settings']['field_injection'])) {
              $bundles[] = $bundle;
            }
          }
          if ($bundles) {
            // We should check a relationship through this field, only for
            // nodes of the collected bundles.
            $product_reference_fields[$field_name] = $bundles;
          }
        }
      }
    }

    $nodes = array();
    foreach ($product_reference_fields as $field_name => $bundles) {
      $query = new EntityFieldQuery();
      $result = $query->entityCondition('entity_type', 'node')
        ->entityCondition('bundle', $bundles, 'IN')
        ->propertyCondition('status', 1)
        ->fieldCondition($field_name, 'product_id', $product_id)
        ->execute();

      if (isset($result['node'])) {
        $nodes = array_merge($nodes, array_keys($result['node']));
      }
    }

    $nodes = array_unique($nodes);
    if ($nodes) {
      $results = db_query('SELECT tid FROM {taxonomy_index} WHERE nid in (:nids)', array(':nids' => $nodes))->fetchCol();
      foreach ($results as $tid) {
        $cached_items[$cache_key][$tid] = $tid;
      }
    }

  }

  if (is_array($cached_items[$cache_key]) && !empty(array_intersect($cached_items[$cache_key], $terms_to_check))) {
    return TRUE;
  }

  // Return false by default.
  return FALSE;
}
