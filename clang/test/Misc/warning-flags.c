RUN: diagtool list-warnings 2>&1 | FileCheck %s

This test serves two purposes:

(1) It documents all existing warnings that currently have no associated -W flag,
    and ensures that the list never grows.
    
    If take an existing warning and add a flag, this test will fail.  To
    fix this test, simply remove that warning from the list below.
    
(2) It prevents us adding new warnings to Clang that have no -W flag.  All
    new warnings should have -W flags.
    
    If you add a new warning without a flag, this test will fail.  To fix
    this test, simply add a warning group to that warning.
    

The list of warnings below should NEVER grow.  It should gradually shrink to 0.

CHECK: Warnings without flags (241):
CHECK-NEXT:   ext_anonymous_struct_union_qualified
CHECK-NEXT:   ext_binary_literal
CHECK-NEXT:   ext_cast_fn_obj
CHECK-NEXT:   ext_delete_void_ptr_operand
CHECK-NEXT:   ext_designated_init
CHECK-NEXT:   ext_duplicate_declspec
CHECK-NEXT:   ext_ellipsis_exception_spec
CHECK-NEXT:   ext_empty_fnmacro_arg
CHECK-NEXT:   ext_enum_friend
CHECK-NEXT:   ext_enum_value_not_int
CHECK-NEXT:   ext_enumerator_list_comma
CHECK-NEXT:   ext_expected_semi_decl_list
CHECK-NEXT:   ext_explicit_instantiation_without_qualified_id
CHECK-NEXT:   ext_explicit_specialization_storage_class
CHECK-NEXT:   ext_forward_ref_enum
CHECK-NEXT:   ext_freestanding_complex
CHECK-NEXT:   ext_hexconstant_invalid
CHECK-NEXT:   ext_ident_list_in_param
CHECK-NEXT:   ext_imaginary_constant
CHECK-NEXT:   ext_implicit_lib_function_decl
CHECK-NEXT:   ext_in_class_initializer_non_constant
CHECK-NEXT:   ext_integer_complement_complex
CHECK-NEXT:   ext_integer_complex
CHECK-NEXT:   ext_integer_increment_complex
CHECK-NEXT:   ext_invalid_sign_spec
CHECK-NEXT:   ext_missing_declspec
CHECK-NEXT:   ext_missing_varargs_arg
CHECK-NEXT:   ext_missing_whitespace_after_macro_name
CHECK-NEXT:   ext_new_paren_array_nonconst
CHECK-NEXT:   ext_nonstandard_escape
CHECK-NEXT:   ext_param_not_declared
CHECK-NEXT:   ext_paste_comma
CHECK-NEXT:   ext_plain_complex
CHECK-NEXT:   ext_pp_bad_vaargs_use
CHECK-NEXT:   ext_pp_comma_expr
CHECK-NEXT:   ext_pp_ident_directive
CHECK-NEXT:   ext_pp_include_next_directive
CHECK-NEXT:   ext_pp_line_too_big
CHECK-NEXT:   ext_pp_macro_redef
CHECK-NEXT:   ext_pp_warning_directive
CHECK-NEXT:   ext_return_has_void_expr
CHECK-NEXT:   ext_subscript_non_lvalue
CHECK-NEXT:   ext_template_arg_extra_parens
CHECK-NEXT:   ext_thread_before
CHECK-NEXT:   ext_typecheck_addrof_void
CHECK-NEXT:   ext_typecheck_cast_nonscalar
CHECK-NEXT:   ext_typecheck_cast_to_union
CHECK-NEXT:   ext_typecheck_comparison_of_distinct_pointers
CHECK-NEXT:   ext_typecheck_comparison_of_distinct_pointers_nonstandard
CHECK-NEXT:   ext_typecheck_comparison_of_fptr_to_void
CHECK-NEXT:   ext_typecheck_comparison_of_pointer_integer
CHECK-NEXT:   ext_typecheck_cond_incompatible_operands
CHECK-NEXT:   ext_typecheck_cond_incompatible_operands_nonstandard
CHECK-NEXT:   ext_typecheck_cond_one_void
CHECK-NEXT:   ext_typecheck_convert_pointer_void_func
CHECK-NEXT:   ext_typecheck_ordered_comparison_of_function_pointers
CHECK-NEXT:   ext_typecheck_ordered_comparison_of_pointer_and_zero
CHECK-NEXT:   ext_typecheck_ordered_comparison_of_pointer_integer
CHECK-NEXT:   ext_typecheck_zero_array_size
CHECK-NEXT:   ext_unknown_escape
CHECK-NEXT:   ext_using_undefined_std
CHECK-NEXT:   ext_vla_folded_to_constant
CHECK-NEXT:   pp_include_next_absolute_path
CHECK-NEXT:   pp_include_next_in_primary
CHECK-NEXT:   pp_invalid_string_literal
CHECK-NEXT:   pp_out_of_date_dependency
CHECK-NEXT:   pp_poisoning_existing_macro
CHECK-NEXT:   pp_pragma_once_in_main_file
CHECK-NEXT:   pp_pragma_sysheader_in_main_file
CHECK-NEXT:   pp_undef_builtin_macro
CHECK-NEXT:   w_asm_qualifier_ignored
CHECK-NEXT:   warn_accessor_property_type_mismatch
CHECK-NEXT:   warn_anon_bitfield_width_exceeds_type_size
CHECK-NEXT:   warn_asm_label_on_auto_decl
CHECK-NEXT:   warn_attribute_ibaction
CHECK-NEXT:   warn_attribute_iboutlet
CHECK-NEXT:   warn_attribute_ignored
CHECK-NEXT:   warn_attribute_ignored_for_field_of_type
CHECK-NEXT:   warn_attribute_malloc_pointer_only
CHECK-NEXT:   warn_attribute_nonnull_no_pointers
CHECK-NEXT:   warn_attribute_precede_definition
CHECK-NEXT:   warn_attribute_sentinel_named_arguments
CHECK-NEXT:   warn_attribute_sentinel_not_variadic
CHECK-NEXT:   warn_attribute_type_not_supported
CHECK-NEXT:   warn_attribute_unknown_visibility
CHECK-NEXT:   warn_attribute_void_function_method
CHECK-NEXT:   warn_attribute_weak_import_invalid_on_definition
CHECK-NEXT:   warn_attribute_weak_on_field
CHECK-NEXT:   warn_attribute_weak_on_local
CHECK-NEXT:   warn_attribute_wrong_decl_type
CHECK-NEXT:   warn_bad_receiver_type
CHECK-NEXT:   warn_bitfield_width_exceeds_type_size
CHECK-NEXT:   warn_bool_switch_condition
CHECK-NEXT:   warn_braces_around_scalar_init
CHECK-NEXT:   warn_c_kext
CHECK-NEXT:   warn_call_to_pure_virtual_member_function_from_ctor_dtor
CHECK-NEXT:   warn_call_wrong_number_of_arguments
CHECK-NEXT:   warn_case_empty_range
CHECK-NEXT:   warn_char_constant_too_large
CHECK-NEXT:   warn_class_method_not_found
CHECK-NEXT:   warn_cmdline_missing_macro_defs
CHECK-NEXT:   warn_collection_expr_type
CHECK-NEXT:   warn_conflicting_param_types
CHECK-NEXT:   warn_conflicting_ret_types
CHECK-NEXT:   warn_conflicting_variadic
CHECK-NEXT:   warn_conv_to_base_not_used
CHECK-NEXT:   warn_conv_to_self_not_used
CHECK-NEXT:   warn_conv_to_void_not_used
CHECK-NEXT:   warn_delete_array_type
CHECK-NEXT:   warn_double_const_requires_fp64
CHECK-NEXT:   warn_drv_assuming_mfloat_abi_is
CHECK-NEXT:   warn_drv_clang_unsupported
CHECK-NEXT:   warn_drv_input_file_unused
CHECK-NEXT:   warn_drv_not_using_clang_arch
CHECK-NEXT:   warn_drv_not_using_clang_cpp
CHECK-NEXT:   warn_drv_not_using_clang_cxx
CHECK-NEXT:   warn_drv_objc_gc_unsupported
CHECK-NEXT:   warn_drv_pch_not_first_include
CHECK-NEXT:   warn_drv_preprocessed_input_file_unused
CHECK-NEXT:   warn_dup_category_def
CHECK-NEXT:   warn_duplicate_protocol_def
CHECK-NEXT:   warn_enum_too_large
CHECK-NEXT:   warn_enum_value_overflow
CHECK-NEXT:   warn_enumerator_too_large
CHECK-NEXT:   warn_exception_caught_by_earlier_handler
CHECK-NEXT:   warn_excess_initializers
CHECK-NEXT:   warn_excess_initializers_in_char_array_initializer
CHECK-NEXT:   warn_expected_qualified_after_typename
CHECK-NEXT:   warn_extern_init
CHECK-NEXT:   warn_extraneous_char_constant
CHECK-NEXT:   warn_fe_cc_log_diagnostics_failure
CHECK-NEXT:   warn_fe_cc_print_header_failure
CHECK-NEXT:   warn_fe_macro_contains_embedded_newline
CHECK-NEXT:   warn_file_asm_volatile
CHECK-NEXT:   warn_function_attribute_wrong_type
CHECK-NEXT:   warn_gc_attribute_weak_on_local
CHECK-NEXT:   warn_gnu_inline_attribute_requires_inline
CHECK-NEXT:   warn_hex_escape_too_large
CHECK-NEXT:   warn_ignoring_ftabstop_value
CHECK-NEXT:   warn_illegal_constant_array_size
CHECK-NEXT:   warn_implements_nscopying
CHECK-NEXT:   warn_incompatible_qualified_id
CHECK-NEXT:   warn_initializer_string_for_char_array_too_long
CHECK-NEXT:   warn_inline_namespace_reopened_noninline
CHECK-NEXT:   warn_inst_method_not_found
CHECK-NEXT:   warn_instance_method_on_class_found
CHECK-NEXT:   warn_integer_too_large
CHECK-NEXT:   warn_integer_too_large_for_signed
CHECK-NEXT:   warn_invalid_asm_cast_lvalue
CHECK-NEXT:   warn_many_braces_around_scalar_init
CHECK-NEXT:   warn_maynot_respond
CHECK-NEXT:   warn_member_extra_qualification
CHECK-NEXT:   warn_method_param_redefinition
CHECK-NEXT:   warn_mismatched_exception_spec
CHECK-NEXT:   warn_missing_case_for_condition
CHECK-NEXT:   warn_missing_dependent_template_keyword
CHECK-NEXT:   warn_missing_exception_specification
CHECK-NEXT:   warn_missing_whitespace_after_macro_name
CHECK-NEXT:   warn_multiple_method_decl
CHECK-NEXT:   warn_no_constructor_for_refconst
CHECK-NEXT:   warn_nonnull_pointers_only
CHECK-NEXT:   warn_not_compound_assign
CHECK-NEXT:   warn_ns_attribute_wrong_parameter_type
CHECK-NEXT:   warn_ns_attribute_wrong_return_type
CHECK-NEXT:   warn_objc_object_attribute_wrong_type
CHECK-NEXT:   warn_objc_property_copy_missing_on_block
CHECK-NEXT:   warn_objc_protocol_qualifier_missing_id
CHECK-NEXT:   warn_octal_escape_too_large
CHECK-NEXT:   warn_odr_tag_type_inconsistent
CHECK-NEXT:   warn_on_superclass_use
CHECK-NEXT:   warn_param_default_argument_redefinition
CHECK-NEXT:   warn_partial_specs_not_deducible
CHECK-NEXT:   warn_pointer_attribute_wrong_type
CHECK-NEXT:   warn_pp_convert_lhs_to_positive
CHECK-NEXT:   warn_pp_convert_rhs_to_positive
CHECK-NEXT:   warn_pp_expr_overflow
CHECK-NEXT:   warn_pp_line_decimal
CHECK-NEXT:   warn_pragma_align_expected_equal
CHECK-NEXT:   warn_pragma_align_invalid_option
CHECK-NEXT:   warn_pragma_debug_unexpected_command
CHECK-NEXT:   warn_pragma_expected_colon
CHECK-NEXT:   warn_pragma_expected_enable_disable
CHECK-NEXT:   warn_pragma_expected_identifier
CHECK-NEXT:   warn_pragma_expected_lparen
CHECK-NEXT:   warn_pragma_expected_rparen
CHECK-NEXT:   warn_pragma_extra_tokens_at_eol
CHECK-NEXT:   warn_pragma_ms_struct
CHECK-NEXT:   warn_pragma_options_align_reset_failed
CHECK-NEXT:   warn_pragma_options_align_unsupported_option
CHECK-NEXT:   warn_pragma_options_expected_align
CHECK-NEXT:   warn_pragma_pack_invalid_action
CHECK-NEXT:   warn_pragma_pack_invalid_alignment
CHECK-NEXT:   warn_pragma_pack_malformed
CHECK-NEXT:   warn_pragma_pack_pop_failed
CHECK-NEXT:   warn_pragma_pack_pop_identifer_and_alignment
CHECK-NEXT:   warn_pragma_pack_show
CHECK-NEXT:   warn_pragma_pop_macro_no_push
CHECK-NEXT:   warn_pragma_unknown_extension
CHECK-NEXT:   warn_pragma_unused_expected_punc
CHECK-NEXT:   warn_pragma_unused_expected_var
CHECK-NEXT:   warn_pragma_unused_expected_var_arg
CHECK-NEXT:   warn_pragma_unused_undeclared_var
CHECK-NEXT:   warn_previous_alias_decl
CHECK-NEXT:   warn_printf_asterisk_missing_arg
CHECK-NEXT:   warn_property_attr_mismatch
CHECK-NEXT:   warn_property_attribute
CHECK-NEXT:   warn_property_getter_owning_mismatch
CHECK-NEXT:   warn_property_types_are_incompatible
CHECK-NEXT:   warn_readonly_property
CHECK-NEXT:   warn_receiver_forward_class
CHECK-NEXT:   warn_redecl_library_builtin
CHECK-NEXT:   warn_redeclaration_without_attribute_prev_attribute_ignored
CHECK-NEXT:   warn_register_objc_catch_parm
CHECK-NEXT:   warn_related_result_type_compatibility_class
CHECK-NEXT:   warn_related_result_type_compatibility_protocol
CHECK-NEXT:   warn_root_inst_method_not_found
CHECK-NEXT:   warn_second_parameter_of_va_start_not_last_named_argument
CHECK-NEXT:   warn_second_parameter_to_va_arg_never_compatible
CHECK-NEXT:   warn_standalone_specifier
CHECK-NEXT:   warn_static_inline_explicit_inst_ignored
CHECK-NEXT:   warn_static_non_static
CHECK-NEXT:   warn_template_export_unsupported
CHECK-NEXT:   warn_template_spec_extra_headers
CHECK-NEXT:   warn_tentative_incomplete_array
CHECK-NEXT:   warn_transparent_union_attribute_field_size_align
CHECK-NEXT:   warn_transparent_union_attribute_floating
CHECK-NEXT:   warn_transparent_union_attribute_not_definition
CHECK-NEXT:   warn_transparent_union_attribute_zero_fields
CHECK-NEXT:   warn_typecheck_function_qualifiers
CHECK-NEXT:   warn_unavailable_fwdclass_message
CHECK-NEXT:   warn_undef_interface
CHECK-NEXT:   warn_undef_interface_suggest
CHECK-NEXT:   warn_undef_protocolref
CHECK-NEXT:   warn_undefined_internal
CHECK-NEXT:   warn_unknown_analyzer_checker
CHECK-NEXT:   warn_unknown_method_family
CHECK-NEXT:   warn_unterminated_char
CHECK-NEXT:   warn_unterminated_string
CHECK-NEXT:   warn_use_out_of_scope_declaration
CHECK-NEXT:   warn_weak_identifier_undeclared
CHECK-NEXT:   warn_weak_import
