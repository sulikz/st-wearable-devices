function normal		(shader, t_base, t_second, t_detail)
	shader:begin	("wd_crt","wd_crt_vektor")
			: fog		(false)
			: zb 		(true,false)
			: blend		(true,blend.srcalpha,blend.invsrcalpha)
			: color_write_enable( true, true, true, false)
	shader:sampler("s_base"):texture(t_base):clamp():f_linear()
end
