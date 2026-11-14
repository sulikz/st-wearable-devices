-- halo quads behind the digits
function normal(shader, t_base, t_second, t_detail)
	shader:begin	("wd_nixie", "wd_halo")
		: fog		(false)
		: zb		(true,false)
		: blend		(true,blend.one,blend.one)
		: sorting	(1, true)

	shader:sampler("s_base"):texture(t_base):clamp():f_linear()
end
