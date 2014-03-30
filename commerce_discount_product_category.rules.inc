<?php

/**
 * Implements hook_rules_condition_info().
 */
function commerce_discount_product_category_rules_condition_info() {
  $inline_conditions = inline_conditions_get_info();
  $conditions = array();

  $conditions['commerce_product_display_has_specified_terms'] = array(
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
    'module' => 'inline_conditions',
    'group' => t('Commerce Line Item'),
    'callbacks' => array(
      'execute' => 'commerce_product_display_has_specified_terms_build',
    ),
  );

  return $conditions;
}
