<?php
/**
 * @file
 * Provides implementation of inline_conditions.
 */

/**
 * Implements hook_inline_conditions_info().
 */
function commerce_discount_product_category_inline_conditions_info() {
  $conditions = array();

  $conditions['commerce_product_display_has_specified_terms'] = array(
    'label' => t('Product category'),
    'entity type' => 'commerce_line_item',
    'callbacks' => array(
      'configure' => 'commerce_product_display_has_specified_terms_configure',
      'build' => 'commerce_product_display_has_specified_terms_build',
    ),
  );

  return $conditions;
}

/**
 * Configuration callback for commerce_product_has_specified_terms on product.
 *
 * @param array $settings
 *   Values for the form element.
 *
 * @return array
 *   Return a form element.
 */
function commerce_product_display_has_specified_terms_configure($settings) {
  $form = array();

  $default_value = '';
  if (!empty($settings['terms'])) {
    foreach ($settings['terms'] as $delta => $term) {
      $default_value .= taxonomy_term_load($term['target_id'])->name . ' (' . $term['target_id'] . ')';
      if ($term != end($settings['terms'])) {
        $default_value .= ', ';
      }
    }
  }

  $form['terms'] = array(
    '#type' => 'textfield',
    '#title' => t('Terms'),
    '#title_display' => 'invisible',
    '#required' => TRUE,
    '#maxlength' => 4096,
    '#default_value' => $default_value,
    '#autocomplete_path' => 'inline_conditions/autocomplete/taxonomy_term/0/0',
    '#element_validate' => array('_inline_conditions_autocomplete_validate'),
    '#suffix' => '<div class="condition-instructions">' . t('The discount is active if the product has the selected term(s).') . '</div>',
  );

  return $form;
}
