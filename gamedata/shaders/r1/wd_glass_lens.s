function normal(shader, t_base, t_second, t_detail)
	shader:begin("wd_glass", "wd_glass_lens")
		: fog(false)
		: zb(true, false)
		: blend(true, blend.srcalpha, blend.invsrcalpha)
		: aref(true, 0)
		: sorting(2, true)
	shader:sampler("s_base"):texture(t_base):clamp():f_linear()
end
