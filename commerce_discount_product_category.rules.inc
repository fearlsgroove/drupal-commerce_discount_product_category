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
  $terms_to_check = explode(', ', $terms);

  // If commerce_product is attached.
  if (in_array('commerce_product', array_keys($wrapper->getPropertyInfo()))) {
    // Make sure we have a bridge to a display node.
    foreach ($wrapper->commerce_product->getPropertyInfo() as $field_name => $property) {

      if (isset($property['field']) && $property['field']['type'] == 'commerce_product_reference') {
        if ($wrapper->commerce_product->$field_name instanceof EntityListWrapper) {

          // Cycle through the product display reference to find term fields.
          foreach ($wrapper->commerce_product->$field_name as $node_display_wrapper) {
            foreach ($node_display_wrapper->getPropertyInfo() as $node_field_name => $node_property) {

              // Check if the property is of a taxonomy term type.
              if (isset($node_property['type']) && $node_property['type'] == 'taxonomy_term') {

                // If the wrapped field is an instance of EntityListWrapper class, that
                // means that field contains multiple values.
                if ($node_display_wrapper->$node_field_name instanceof EntityListWrapper) {
                  foreach ($node_display_wrapper->$node_field_name as $wrapper_term) {
                    // We have a match, apply discount
                    if (($key = array_search($wrapper_term->getIdentifier(), $terms_to_check)) !== FALSE) {
                      return TRUE;
                    }
                  }
                }
                elseif ($term =$node_display_wrapper->$node_field_name->value()) {
                  // The single value matches, return true.
                  if (($key = array_search($term->tid, $terms_to_check)) !== FALSE) {
                    return TRUE;
                  }
                }
              }
            }
          }
        }
      }
    }
  }
  // Return false by default.
  return FALSE;
}
