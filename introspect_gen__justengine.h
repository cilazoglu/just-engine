#pragma once

#ifndef PRE_INTROSPECT_PASS

#include "justengine.h"

/**
 * !! IMPORTANT !!
 * IMPORT THIS HEADER AFTER THESE DEFINITONS
 * - URectSize
 * - StepTimer
 * - SpriteSheetAnimationState
 * - AABBCollider
 * - AABBColliderSet
*/

static FieldInfo URectSize__fields[2];
static FieldInfo StepTimer__fields[4];
static FieldInfo SpriteSheetAnimationState__fields[8];
static FieldInfo AABBCollider__fields[4];
static FieldInfo AABBColliderSet__fields[4];

static FieldInfo URectSize__union_0__variants[] = {
	{
		.type = TYPE_uint32, .name = "width", .ptr = 0,
	},
	{
		.type = TYPE_uint32, .name = "x", .ptr = 0,
	},
};

static FieldInfo URectSize__union_1__variants[] = {
	{
		.type = TYPE_uint32, .name = "height", .ptr = 0,
	},
	{
		.type = TYPE_uint32, .name = "y", .ptr = 0,
	},
};

static FieldInfo URectSize__fields[] = {
	{
		.type = TYPE_union, .name = "width", .ptr = &(((URectSize*)(0))->width),
		.is_named_union = false, .union_name = "(null)", .union_header_variant = 0,
		.union_size = sizeof(union { uint32 width; uint32 x; }), .variant_count = ARRAY_LENGTH(URectSize__union_0__variants), .variants = URectSize__union_0__variants,
	},
	{
		.type = TYPE_union, .name = "height", .ptr = &(((URectSize*)(0))->height),
		.is_named_union = false, .union_name = "(null)", .union_header_variant = 0,
		.union_size = sizeof(union { uint32 height; uint32 y; }), .variant_count = ARRAY_LENGTH(URectSize__union_1__variants), .variants = URectSize__union_1__variants,
	},
};

static FieldInfo StepTimer__fields[] = {
	{
		.type = TYPE_uint32, .name = "mode", .ptr = &(((StepTimer*)(0))->mode),
	},
	{
		.type = TYPE_uint32, .name = "step_count", .ptr = &(((StepTimer*)(0))->step_count),
	},
	{
		.type = TYPE_uint32, .name = "current_step", .ptr = &(((StepTimer*)(0))->current_step),
	},
	{
		.type = TYPE_bool, .name = "finished", .ptr = &(((StepTimer*)(0))->finished),
	},
};

static FieldInfo SpriteSheetAnimationState__fields[] = {
	{
		.type = TYPE_struct, .name = "texture_offset", .ptr = &(((SpriteSheetAnimationState*)(0))->texture_offset),
		.struct_size = sizeof(URectSize), .field_count = ARRAY_LENGTH(URectSize__fields), .fields = URectSize__fields,
	},
	{
		.type = TYPE_struct, .name = "sprite_size", .ptr = &(((SpriteSheetAnimationState*)(0))->sprite_size),
		.struct_size = sizeof(URectSize), .field_count = ARRAY_LENGTH(URectSize__fields), .fields = URectSize__fields,
	},
	{
		.type = TYPE_uint32, .name = "rows", .ptr = &(((SpriteSheetAnimationState*)(0))->rows),
	},
	{
		.type = TYPE_uint32, .name = "cols", .ptr = &(((SpriteSheetAnimationState*)(0))->cols),
	},
	{
		.type = TYPE_uint32, .name = "frame_count", .ptr = &(((SpriteSheetAnimationState*)(0))->frame_count),
	},
	{
		.type = TYPE_struct, .name = "timer", .ptr = &(((SpriteSheetAnimationState*)(0))->timer),
		.struct_size = sizeof(StepTimer), .field_count = ARRAY_LENGTH(StepTimer__fields), .fields = StepTimer__fields,
	},
	{
		.type = TYPE_uint32, .name = "current_frame", .ptr = &(((SpriteSheetAnimationState*)(0))->current_frame),
	},
	{
		.type = TYPE_bool, .name = "finished", .ptr = &(((SpriteSheetAnimationState*)(0))->finished),
	},
};

static FieldInfo AABBCollider__fields[] = {
	{
		.type = TYPE_float32, .name = "x_left", .ptr = &(((AABBCollider*)(0))->x_left),
	},
	{
		.type = TYPE_float32, .name = "x_right", .ptr = &(((AABBCollider*)(0))->x_right),
	},
	{
		.type = TYPE_float32, .name = "y_top", .ptr = &(((AABBCollider*)(0))->y_top),
	},
	{
		.type = TYPE_float32, .name = "y_bottom", .ptr = &(((AABBCollider*)(0))->y_bottom),
	},
};

static FieldInfo AABBColliderSet__fields[] = {
	{
		.type = TYPE_usize, .name = "count", .ptr = &(((AABBColliderSet*)(0))->count),
	},
	{
		.type = TYPE_usize, .name = "capacity", .ptr = &(((AABBColliderSet*)(0))->capacity),
	},
	{
		.type = TYPE_struct, .name = "bounding_box", .ptr = &(((AABBColliderSet*)(0))->bounding_box),
		.struct_size = sizeof(AABBCollider), .field_count = ARRAY_LENGTH(AABBCollider__fields), .fields = AABBCollider__fields,
	},
	{
		.type = TYPE_struct, .name = "colliders", .ptr = &(((AABBColliderSet*)(0))->colliders),
		.is_ptr = true, .ptr_depth = 1,
		.is_dynarray = true, .count_ptr = &(((AABBColliderSet*)(0))->count),
		.struct_size = sizeof(AABBCollider), .field_count = ARRAY_LENGTH(AABBCollider__fields), .fields = AABBCollider__fields,
	},
};


__IMPL_____generate_print_functions(URectSize);
__IMPL_____generate_print_functions(StepTimer);
__IMPL_____generate_print_functions(SpriteSheetAnimationState);
__IMPL_____generate_print_functions(AABBCollider);
__IMPL_____generate_print_functions(AABBColliderSet);


#endif

