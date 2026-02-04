/**************************************************************************/
/*  sphere_shape_3d.cpp                                                   */
/**************************************************************************/
/*                         This file is part of:                          */
/*                             GODOT ENGINE                               */
/*                        https://godotengine.org                         */
/**************************************************************************/
/* Copyright (c) 2014-present Godot Engine contributors (see AUTHORS.md). */
/* Copyright (c) 2007-2014 Juan Linietsky, Ariel Manzur.                  */
/*                                                                        */
/* Permission is hereby granted, free of charge, to any person obtaining  */
/* a copy of this software and associated documentation files (the        */
/* "Software"), to deal in the Software without restriction, including    */
/* without limitation the rights to use, copy, modify, merge, publish,    */
/* distribute, sublicense, and/or sell copies of the Software, and to     */
/* permit persons to whom the Software is furnished to do so, subject to  */
/* the following conditions:                                              */
/*                                                                        */
/* The above copyright notice and this permission notice shall be         */
/* included in all copies or substantial portions of the Software.        */
/*                                                                        */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,        */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF     */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. */
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY   */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,   */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE      */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                 */
/**************************************************************************/

#include "sphere_shape_3d.h"

#include "scene/resources/3d/primitive_meshes.h"
#include "servers/physics_3d/physics_server_3d.h"

Vector<Vector3> SphereShape3D::get_debug_mesh_lines() const {
	float r = get_radius();

	Vector<Vector3> points;

	for (int i = 0; i <= 360; i++) {
		float ra = Math::deg_to_rad((float)i);
		float rb = Math::deg_to_rad((float)i + 1);
		Point2 a = Vector2(Math::sin(ra), Math::cos(ra)) * r;
		Point2 b = Vector2(Math::sin(rb), Math::cos(rb)) * r;

		points.push_back(Vector3(a.x, 0, a.y));
		points.push_back(Vector3(b.x, 0, b.y));
		points.push_back(Vector3(0, a.x, a.y));
		points.push_back(Vector3(0, b.x, b.y));
		points.push_back(Vector3(a.x, a.y, 0));
		points.push_back(Vector3(b.x, b.y, 0));
	}

	return points;
}

Ref<ArrayMesh> SphereShape3D::get_debug_arraymesh_faces(const Color &p_modulate) const {
	Array sphere_array;
	sphere_array.resize(RS::ARRAY_MAX);
	SphereMesh::create_mesh_array(sphere_array, radius, radius * 2, 32);

	Vector<Color> colors;
	const PackedVector3Array &verts = sphere_array[RS::ARRAY_VERTEX];
	const int32_t verts_size = verts.size();
	for (int i = 0; i < verts_size; i++) {
		colors.append(p_modulate);
	}

	Ref<ArrayMesh> sphere_mesh = memnew(ArrayMesh);
	sphere_array[RS::ARRAY_COLOR] = colors;
	sphere_mesh->add_surface_from_arrays(Mesh::PRIMITIVE_TRIANGLES, sphere_array);
	return sphere_mesh;
}

real_t SphereShape3D::get_enclosing_radius() const {
	return radius;
}

void SphereShape3D::_update_shape() {
	PhysicsServer3D::get_singleton()->shape_set_data(get_shape(), radius);
	Shape3D::_update_shape();
}

void SphereShape3D::set_radius(float p_radius) {
	ERR_FAIL_COND_MSG(p_radius < 0, "SphereShape3D radius cannot be negative.");
	radius = p_radius;
	_update_shape();
	emit_changed();
}

float SphereShape3D::get_radius() const {
	return radius;
}

bool SphereShape3D::_set(const StringName &p_name, const Variant &p_property) {
	if (PhysicsServer3D::get_singleton()->shape_set_property(get_shape(), get_class_static(), p_name, p_property)) {
		return true;
	}

	return false;
}

bool SphereShape3D::_get(const StringName &p_name, Variant &r_property) const {
	Variant value = PhysicsServer3D::get_singleton()->shape_get_property(get_shape(), get_class_static(), p_name);
	if (value.get_type() != Variant::NIL) {
		r_property = value;
		return true;
	}

	return false;
}

void SphereShape3D::_get_property_list(List<PropertyInfo> *p_list) const {
	for (const Variant &property : PhysicsServer3D::get_singleton()->shape_get_property_list(get_shape(), get_class_static())) {
		p_list->push_back(PropertyInfo::from_dict(property));
	}
}

void SphereShape3D::_validate_property(PropertyInfo &r_property) const {
	Dictionary current_property = (Dictionary)r_property;
	Dictionary modified_property = PhysicsServer3D::get_singleton()->shape_validate_property(get_shape(), get_class_static(), current_property);
	if (!modified_property.is_empty() && modified_property != current_property) {
		r_property = PropertyInfo::from_dict(modified_property);
	}
}

bool SphereShape3D::_property_can_revert(const StringName &p_name) const {
	return PhysicsServer3D::get_singleton()->shape_property_can_revert(get_shape(), get_class_static(), p_name);
}

bool SphereShape3D::_property_get_revert(const StringName &p_name, Variant &r_property) const {
	Variant reverted_value = PhysicsServer3D::get_singleton()->shape_property_get_revert(get_shape(), get_class_static(), p_name);
	if (reverted_value.get_type() != Variant::NIL) {
		r_property = reverted_value;
		return true;
	}

	return false;
}

void SphereShape3D::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_radius", "radius"), &SphereShape3D::set_radius);
	ClassDB::bind_method(D_METHOD("get_radius"), &SphereShape3D::get_radius);

	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "radius", PROPERTY_HINT_RANGE, "0.001,100,0.001,or_greater,suffix:m"), "set_radius", "get_radius");
}

SphereShape3D::SphereShape3D() :
		Shape3D(PhysicsServer3D::get_singleton()->shape_create(PhysicsServer3D::SHAPE_SPHERE)) {
	set_radius(0.5);
}
